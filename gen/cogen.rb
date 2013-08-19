require "erb"

class LoadERB
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

  def initialize(parent,tmpl,opts={})
    parents.push(parent) if parent
    @tmpl = tmpl
    @erb_path = @tmpl

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

# ----------------------------------------------------------------------

module DefMethod

  def def_method(meth, n_arg, tmpl=nil, opts={})
    h = {:method => meth, :mod_var => mod_var, :n_arg => n_arg}
    h.merge!(opts)
    tmpl ||= meth
    Function.new(self, tmpl, h)
  end

  def def_singleton(meth, n_arg, tmpl=nil, opts={})
    def_method(meth, n_arg, tmpl, :singleton => true)
  end

  def def_alias(dst, src)
    Alias.new(self, dst, src)
  end

  def def_allocate(tmpl)
    h = {:mod_var => mod_var, :method => "allocate", :singleton => true}
    Allocate.new(self, tmpl, h)
  end

  def binary(meth, ope=nil)
    ope = meth if ope.nil?
    def_method(meth, 1, "binary", :op => ope)
  end

  def binary2(meth, ope=nil)
    ope = meth if ope.nil?
    def_method(meth, 1, "binary2", :op =>ope)
  end

  def unary(meth, ope=nil)
    def_method(meth, 0, "unary", :op => ope)
  end

  def pow
    def_method("pow", 1, "pow", :op => "**")
  end

  def unary2(meth, dtype, tpclass)
    h = {:dtype => dtype, :tpclass => tpclass}
    def_method(meth, 0, "unary2", h)
  end

  def set2(meth, dtype, tpclass)
    h = {:dtype => dtype, :tpclass => tpclass}
    def_method(meth, 1, "set2", h)
  end

  def cond_binary(meth,op=nil)
    def_method(meth, 1, "cond_binary", :op => op)
  end

  def cond_unary(meth)
    def_method(meth, 0, "cond_unary")
  end

  def bit_binary(meth, op=nil)
    def_method(meth, 1, "bit_binary", :op => op)
  end

  def bit_unary(meth, op=nil)
    def_method(meth, 0, "bit_unary", :op => op)
  end

  def bit_count(meth)
    def_method(meth, -1, "bit_count")
  end

  def accum(meth)
    def_method(meth, -1, "accum")
  end

  def qsort(tp, dtype, dcast)
    h = {:tp => tp, :dtype => dtype, :dcast => dcast}
    NodefFunction.new(self, "qsort", h)
  end

  def math(meth, n=1)
    h = {:method => meth, :mod_var => 'mTM', :n_arg => n}
    case n
    when 1
      Function.new(self, "unary_s", h)
    when 2
      Function.new(self, "binary_s", h)
    when 3
      Function.new(self, "ternary_s", h)
    else
      raise "invalid n=#{n}"
    end
  end

  def store_numeric
    StoreNum.new(self,"store_numeric")
  end

  def store_array
    StoreArray.new(self,"store_array")
  end

  def cast_array
    CastArray.new(self,"cast_array")
  end

  def store_from(cname,dtype,macro)
    Store.new(self,"store_from",cname.downcase,dtype,"c"+cname,macro)
  end

  def store
    Function.new(self,"store","store")
  end

  def find_method(meth)
    Function::DEFS.find{|x| x.kind_of?(Function) and meth == x.method }
  end

  def find_tmpl(meth)
    Function::DEFS.find{|x| x.kind_of?(Function) and meth == x.tmpl }
  end

  def cast_func
    "nary_#{tp}_s_cast"
  end
end

# ----------------------------------------------------------------------

class DataType < LoadERB
  include DefMethod

  def initialize(erb_path, type_file)
    super(nil, erb_path)
    @class_alias = []
    @upcast = []
    @mod_var = "cT"
    load_type(type_file) if type_file
  end

  def load_type(file)
    s = File.read(file)
    eval(s)
  end

  attrs = %w[
    class_name
    ctype
    blas_char
    complex_class_name
    complex_type
    real_class_name
    real_ctype
    has_math
    is_bit
    is_int
    is_float
    is_real
    is_complex
    is_object
    is_comparable
    mod_var
  ]

  define_attrs attrs

  def type_name
    @type_name ||= class_name.downcase
  end
  alias tp type_name

  def type_var
    @type_var ||= "c"+class_name
  end

  def math_var
    @math_var ||= "m"+class_name+"Math"
  end

  def real_class_name(arg=nil)
    if arg.nil?
      @real_class_name ||= class_name
    else
      @real_class_name = arg
    end
  end

  def real_ctype(arg=nil)
    if arg.nil?
      @real_ctype ||= ctype
    else
      @real_ctype = arg
    end
  end

  def real_type_var
    @real_type_var ||= "c"+real_class_name
  end

  def real_type_name
    @real_type_name ||= real_class_name.downcase
  end

  def class_alias(*args)
    @class_alias.concat(args)
  end

  def upcast(c=nil,t="T")
    if c
      @upcast << "rb_hash_aset(hCast, c#{c}, c#{t});"
    else
      @upcast
    end
  end

  def upcast_rb(c,t="T")
    if c=="Integer"
      @upcast << "rb_hash_aset(hCast, rb_cFixnum, c#{t});"
      @upcast << "rb_hash_aset(hCast, rb_cBignum, c#{t});"
    else
      @upcast << "rb_hash_aset(hCast, rb_c#{c}, c#{t});"
    end
  end
end


# ----------------------------------------------------------------------

class Function < LoadERB
  DEFS = []

  attrs = %w[
    singleton
    method
    mod_var
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
    @erb_path = File.join(TMPL_DIR, tmpl+".c")
    DEFS.push(self)
  end

  def c_iter
    "iter_#{type_name}_#{method}"
  end
  alias c_iterator c_iter

  def c_func
    s = singleton ? "_s" : ""
    "nary_#{type_name}#{s}_#{method}"
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

class NodefFunction < Function
  def definition
    nil
  end
end

class Alias < LoadERB
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
    #mod_var = 'cT'
    "rb_define_alias(#{mod_var}, \"#{dst}\", \"#{src}\");"
  end
end

class Allocate < Function
  def definition
    "rb_define_alloc_func(#{mod_var}, #{c_func});"
  end
end

# ----------------------------------------------------------------------

class Store < Function #LoadERB
  DEFS = []

  def initialize(parent,tmpl,tpname,dtype,tpclass,macro)
    super(parent,tmpl)
    @tpname=tpname
    @dtype=dtype
    @tpclass=tpclass
    @macro=macro
    DEFS.push(self)
  end
  attr_reader :tmpl, :tpname, :dtype, :tpclass, :macro

  def c_func
    "nary_#{tp}_store_#{tpname}"
  end

  def c_iter
    "iter_#{tp}_store_#{tpname}"
  end

  def definition
    nil
  end

  def condition
    "rb_obj_is_kind_of(obj,#{tpclass})"
  end

  def self.definitions
    a = []
    DEFS.each do |i|
      a.push(i) if i.condition
    end
    a
  end
end

class StoreNum < Store
  def initialize(parent,tmpl)
    super(parent,tmpl,"numeric",nil,nil,nil)
  end

  def condition
    "FIXNUM_P(obj) || TYPE(obj)==T_FLOAT || TYPE(obj)==T_BIGNUM || rb_obj_is_kind_of(obj,rb_cComplex)"
  end
end

class StoreArray < Store
  def initialize(parent,tmpl)
    super(parent,tmpl,"array",nil,nil,nil)
  end

  def c_func
    "nary_#{tp}_#{tmpl}"
  end

  def condition
    "TYPE(obj)==T_ARRAY"
  end
end

class CastArray < StoreArray
  def condition
    nil
  end
end

# ----------------------------------------------------------------------

if !((1..2) === ARGV.size)
  puts "usage:\n  ruby #{$0} erb_path [type_file]"
  exit 1
end

erb_path = ARGV[0]
type_file = ARGV[1]

TMPL_DIR = "tmpl"
DataType.new(erb_path, type_file).run
