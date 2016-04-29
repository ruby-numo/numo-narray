require "erb"

class ErbPP
  ATTRS = []

  def self.define_attrs(attrs)
    attrs.each do |attr|
      ivar = ("@"+attr).to_sym
      define_method(attr){|*a| attr_method(ivar,*a)}
    end
  end

  def attr_method(ivar,arg=nil)
    if arg.nil?
      instance_variable_get(ivar)
    else
      instance_variable_set(ivar,arg)
    end
  end

  def initialize(parent,erb_path,opts={})
    parents.push(parent) if parent
    @erb_path = erb_path
    @tmpl = @erb_path

    @opts = opts
    if @opts.class != Hash
      raise ArgumentError, "option is not Hash"
    end

    @opts.each do |k,v|
      ivar = ("@"+k.to_s).to_sym
      instance_variable_set(ivar,v)
    end
  end

  def load_erb
    safe_level = nil
    trim_mode = '%<>'
    @erb = ERB.new(File.read(@erb_path),safe_level,trim_mode)
    @erb.filename = @erb_path
  end

  def parents
    @parents ||= []
  end

  def search_method_in_parents(_meth_id)
    parents.each do |x|
      if x.has_attr? _meth_id
        return x
      end
    end
    parents.each do |x|
      if f = x.search_method_in_parents(_meth_id)
        return f
      end
    end
    nil
  end

  def attrs
    self.class::ATTRS
  end

  def has_attr?(_meth_id)
    respond_to?(_meth_id) or attrs.include?(_meth_id.to_s)
  end

  alias method_missing_alias method_missing

  def method_missing(_meth_id, *args, &block)
    ivar = "@"+_meth_id.to_s
    if args.empty? and instance_variable_defined?(ivar)
      instance_variable_get(ivar)
    elsif args.size == 1 and attrs.include?(_meth_id.to_s)
      instance_variable_set(ivar,args.first)
    elsif x = search_method_in_parents(_meth_id)
      x.send(_meth_id, *args, &block)
    else
      method_missing_alias(_meth_id, *args)
    end
  end

  def run
    load_erb unless @erb
    @erb.run(binding)
  end

  def result
    load_erb unless @erb
    @erb.result(binding)
  end
end
class IdVar
  DEFS = []

  def id_decl
    "static ID id_#{@method};"
  end

  def id_assign
    "id_#{@method} = rb_intern(\"#{@method}\");"
  end

  def initialize(parent,meth)
    @method = meth
    DEFS.push(self)
  end

  def self.declaration
    DEFS.map do |x|
      x.id_decl
    end
  end

  def self.assignment
    DEFS.map do |x|
      x.id_assign
    end
  end
end

# ----------------------------------------------------------------------

class Function < ErbPP
  DEFS = []

  attrs = %w[
    singleton
    method
    n_arg
  ]
  define_attrs attrs

  def id_op
    if op.size == 1
      "'#{op}'"
    else
      "id_#{method}"
    end
  end

  def initialize(parent,tmpl,*opts)
    super
    @aliases = []
    @erb_path = File.join(parent.tmpl_dir, tmpl+".c")
    DEFS.push(self)
  end

  def c_method
    "#{m_prefix}#{method}"
  end

  def c_iter
    begin
      t = "_"+type_name
    rescue
      t = ""
    end
    "iter#{t}_#{method}"
  end
  alias c_iterator c_iter

  def c_func
    s = singleton ? "_s" : ""
    begin
      t = "_"+type_name
    rescue
      t = ""
    end
    "numo#{t}#{s}_#{method}"
  end
  alias c_function c_func
  alias c_instance_method c_func

  def op_map
    @op || method
  end

  def code
    result + "\n\n"
  end

  def definition
    s = singleton ? "_singleton" : ""
    m = op_map
    "rb_define#{s}_method(#{mod_var}, \"#{m}\", #{c_func}, #{n_arg});"
  end

  def self.codes
    a = []
    DEFS.each do |i|
      x = i.code
      a.push(x) if x
    end
    a
  end

  def self.definitions
    a = []
    DEFS.each do |i|
      x = i.definition
      a.push(x) if x
    end
    a
  end
end

class ModuleFunction < Function
  def definition
    m = op_map
    "rb_define_module_function(#{mod_var}, \"#{m}\", #{c_func}, #{n_arg});"
  end
end

class NodefFunction < Function
  def definition
    nil
  end
end

class Alias < ErbPP
  def initialize(parent, dst, src)
    super(parent,nil)
    @dst = dst
    @src = src
    Function::DEFS.push(self)
  end

  def code
    nil
  end

  def definition
    "rb_define_alias(#{mod_var}, \"#{dst}\", \"#{src}\");"
  end
end
