require 'erbpp'

module DefMethod

  def def_id(meth,var=nil)
    IdVar.new(self, meth, var)
  end

  def def_method(meth, n_arg, tmpl=nil, opts={})
    h = {:method => meth, :n_arg => n_arg}
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
    h = {:method => "allocate", :singleton => true}
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
    h = {:op=>op}
    h[:aliases] = [meth] if op
    def_method(meth, 1, "bit_binary", h)
  end

  def bit_unary(meth, op=nil)
    h = {:op=>op}
    h[:aliases] = [meth] if op
    def_method(meth, 0, "bit_unary", h)
  end

  def bit_count(meth)
    def_method(meth, -1, "bit_count")
  end

  def accum(meth, dtype, tpclass)
    h = {:dtype => dtype, :tpclass => tpclass}
    def_method(meth, -1, "accum", h)
  end

  def accum_index(meth)
    def_method(meth, -1, "accum_index")
  end

  def cum(meth, cmacro)
    def_method(meth, -1, "cum", cmacro:cmacro)
  end

  def accum_binary(meth, ope=nil)
    ope = meth if ope.nil?
    def_method(meth, -1, "accum_binary", :op => ope)
  end

  def qsort(tp, dtype, dcast)
    h = {:tp => tp, :dtype => dtype, :dcast => dcast}
    NodefFunction.new(self, "qsort", h)
  end

  def math(meth, n=1)
    h = {:method => meth, :mod_var => 'mTM', :n_arg => n}
    case n
    when 1
      ModuleFunction.new(self, "unary_s", h)
    when 2
      ModuleFunction.new(self, "binary_s", h)
    when 3
      ModuleFunction.new(self, "ternary_s", h)
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
    Store.new(self,"store_from",cname.downcase,dtype,"numo_c"+cname,macro)
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
    "numo_#{tp}_s_cast"
  end
end

# ----------------------------------------------------------------------

class DataType < ErbPP
  include DefMethod

  def initialize(erb_path, type_file)
    super(nil, erb_path)
    @class_alias = []
    @upcast = []
    @mod_var = "cT"
    @tmpl_dir = File.join(File.dirname(erb_path),"tmpl")
    load_type(type_file) if type_file
  end

  attr_reader :tmpl_dir

  def load_type(file)
    eval File.read(file)
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
    @type_var ||= "numo_c"+class_name
  end

  def math_var
    @math_var ||= "numo_m"+class_name+"Math"
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
    @real_type_var ||= "numo_c"+real_class_name
  end

  def real_type_name
    @real_type_name ||= real_class_name.downcase
  end

  def class_alias(*args)
    @class_alias.concat(args)
  end

  def upcast(c=nil,t=nil)
    if c
      if t
        t = "numo_c#{t}"
      else
        t = "cT"
      end
      @upcast << "rb_hash_aset(hCast, numo_c#{c}, #{t});"
    else
      @upcast
    end
  end

  def upcast_rb(c,t=nil)
    if t
      t = "numo_c#{t}"
    else
      t = "cT"
    end
    if c=="Integer"
      if defined?(Fixnum) && Fixnum != Integer
        @upcast << "rb_hash_aset(hCast, rb_cFixnum, #{t});"
        @upcast << "rb_hash_aset(hCast, rb_cBignum, #{t});"
      else
        # RUBY_VERSION >= "2.4.0"
        @upcast << "rb_hash_aset(hCast, rb_cInteger, #{t});"
      end
    else
      @upcast << "rb_hash_aset(hCast, rb_c#{c}, #{t});"
    end
  end
end


# ----------------------------------------------------------------------


class Allocate < Function
  def definition
    "rb_define_alloc_func(#{mod_var}, #{c_func});"
  end
end

# ----------------------------------------------------------------------

class Store < Function
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
    "numo_#{tp}_store_#{tpname}"
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
    "numo_#{tp}_#{tmpl}"
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
