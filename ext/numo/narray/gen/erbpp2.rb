require "erb"

class ErbPP

  def initialize(parent=nil, erb_base=nil, **opts, &block)
    @parent = parent
    @children = []
    @opts = opts
    set erb_base: erb_base if erb_base
    @parent.add_child(self) if @parent
    instance_eval(&block) if block
  end

  attr_reader :children
  attr_accessor :parent

  def add_child(child)
    @children.push(child)
  end

  def set(**opts)
    @opts.merge!(opts)
  end

  def get(key, *args, &block)
    if respond_to?(key)
      return send(key, *args, &block)
    end
    if args.empty? && block.nil? && @opts.has_key?(key)
      return @opts[key]
    end
    if @parent
      return @parent.get(key, *args, &block)
    end
    nil
  end

  def description
    if s = @opts[:description] || @opts[:desc]
      s.gsub(/\@\{/,"[").gsub(/\@\}/,"]")
    end
  end

  alias desc description

  alias method_missing_alias method_missing

  def method_missing(_meth_id, *args, &block)
    if args.empty?
      #$stderr.puts _meth_id.inspect
      v = get(_meth_id, *args, &block)
      return v if !v.nil?
    end
    method_missing_alias(_meth_id, *args, &block)
  end

  # ERB Loader

  def load_erb(base_name)
    safe_level = nil
    trim_mode = '%<>'
    file = base_name + get(:erb_suffix)
    dirs = get(:erb_dir)
    dirs = [dirs] if !dirs.kind_of?(Array)
    dirs.each do |x|
      Dir.glob(x).each do |dir|
        path = File.join(dir,file)
        if File.exist?(path)
          erb = ERB.new(File.read(path), safe_level, trim_mode)
          erb.filename = path
          return erb
        end
      end
    end
    raise "file not found: #{file.inspect} in #{dirs.inspect}"
  end

  def run
    if base = @opts[:erb_base]
      load_erb(base).run(binding)
    end
  end

  def result
    if base = @opts[:erb_base]
      load_erb(base).result(binding)
    end
  end

  def write(output)
    File.open(output,"wt") do |f|
      f.print(result)
    end
  end

  def init_def
  end

  def find_tmpl(name)
    @parent.children.find{|x| x.name == name }
  end

  def find(name)
    children.find{|x| x.name == name }
  end
end


class DefLib < ErbPP
  def initialize(parent=nil, **opts, &block)
    opts[:erb_base] ||= 'lib'
    super(parent, **opts, &block)
  end
  def id_assign
    ids = []
    @children.each{|c| a=c.get(:id_list); ids.concat(a) if a}
    ids.sort.uniq.map{|x| "id_#{x[1]} = rb_intern(\"#{x[0]}\");"}
  end
  def id_decl
    ids = []
    @children.each{|c| a=c.get(:id_list); ids.concat(a) if a}
    ids.sort.uniq.map{|x| "static ID id_#{x[1]};\n"}
  end
  def def_class(**opts, &block)
    DefClass.new(self, **opts, &block)
  end
  def def_module(**opts, &block)
    DefModule.new(self, **opts, &block)
  end
end

module DeclMethod
  def def_alloc_func(m, erb_path=nil, **opts, &block)
    DefAllocFunc.new(self, erb_path||m, name:m, singleton:true, **opts, &block)
  end
  def undef_alloc_func
    UndefAllocFunc.new(self)
  end
  def def_method(m, erb_path=nil, **opts, &block)
    DefMethod.new(self, erb_path||m, name:m, **opts, &block)
  end
  def def_singleton_method(m, erb_path=nil, **opts, &block)
    DefMethod.new(self, erb_path||m, name:m, singleton:true, **opts, &block)
  end
  def def_module_function(m, erb_path=nil, **opts, &block)
    DefModuleFunction.new(self, erb_path||m, name:m, **opts, &block)
  end
  def def_alias(from, to)
    DefAlias.new(self, from:from, to:to)
  end
  def def_const(m, v, **opts, &block)
    DefConst.new(self, name:m, value:v, **opts, &block)
  end
end

class DefModule < ErbPP
  include DeclMethod
  def initialize(parent, **opts, &block)
    eb = opts[:erb_base] || 'module'
    super(parent, erb_base:eb, **opts, &block)
  end
  def id_list
    @id_list ||= []
  end
  def def_id(name,var=nil)
    var = name.gsub(/\?/,"_p").gsub(/\!/,"_bang") if var.nil?
    id_list << [name,var]
  end
  def init_def
    load_erb(init_erb).result(binding)
  end
  def init_erb
    @opts[:init_erb] || "init_module"
  end
  def method_code
    @children.map{|c| c.result}.join("\n")
  end
  def _mod_var
    @opts[:module_var]
  end
end

class DefClass < DefModule
  def initialize(parent, **opts, &block)
    eb = opts[:erb_base] || 'class'
    super(parent, erb_base:eb, **opts, &block)
  end
  def _mod_var
    @opts[:class_var]
  end
  def init_erb
    @opts[:init_erb] || "init_class"
  end
  def super_class
    @opts[:super_class] || "rb_cObject"
  end
  def free_func
    @opts[:free_func] || "gsl_"+get(:name)+"_free"
  end
end

class DefMethod < ErbPP
  include DeclMethod

  def initialize(parent, erb_base, **opts, &block)
    super(parent, **opts, &block)
    set erb_base: erb_base
  end

  def id_op
    if op.size == 1
      "'#{op}'"
    else
      "id_#{c_name}"
    end
  end

  def c_name
    @opts[:name].gsub(/\?/,"_p").gsub(/\!/,"_bang")
  end

  def op_map
    @opts[:op] || @opts[:name]
  end

  def c_func(n_arg=nil)
    set n_arg: n_arg if n_arg
    s = (singleton) ? "_s" : ""
    "#{@parent.name}#{s}_#{c_name}"
  end

  def c_iter
    "iter_#{c_func}"
  end

  def define_method_args
    "#{_mod_var}, \"#{op_map}\", #{c_func}, #{n_arg}"
  end

  def init_def
    return if n_arg == :nodef
    s = (singleton) ? "_singleton" : ""
    "rb_define#{s}_method(#{define_method_args});"
  end

  def singleton
    @opts[:singleton]
  end
end

class DefModuleFunction < DefMethod
  def initialize(parent, erb_base, **opts, &block)
    super(parent, erb_base, **opts, &block)
    set singleton: true
  end

  def init_def
    return if n_arg == :nodef
    "rb_define_module_function(#{define_method_args});"
  end
end

class DefAlias < ErbPP
  def init_def
    "rb_define_alias(#{_mod_var}, \"#{from}\", \"#{to}\");"
  end
end

class DefAllocFunc < DefMethod
  def init_def
    "rb_define_alloc_func(#{_mod_var}, #{c_func});"
  end
end

class UndefAllocFunc < ErbPP
  def init_def
    "rb_undef_alloc_func(#{_mod_var});"
  end
end

class DefConst < ErbPP
  def init_def
    "/*#{desc}*/
    rb_define_const(#{_mod_var},\"#{name}\",#{value});"
  end
end

class DefStruct < ErbPP
  def method_code
    "static VALUE #{class_var};"
  end
  def init_def
    items = members.map{|s| "\"#{s}\""}.join(",")
    "/*#{description}*/
    #{class_var} = rb_struct_define(\"#{class_name}\",#{items},NULL);"
  end
end

class DefInclueModule < ErbPP
  def initialize(parent=nil, incl_class, incl_module, **opts, &block)
    super(parent,incl_class:incl_class,incl_module:incl_module,**opts,&block)
  end
  def init_def
    "rb_include_module(#{get(:incl_class)}, #{get(:incl_module)});"
  end
end
