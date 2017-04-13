require 'erbpp'

module DefMethod

  def def_id(meth,var=nil)
    IdVar.new(self, meth, var)
  end

  def def_method(meth, n_arg, tmpl=nil, opts={})
    h = {:meth => meth, :n_arg => n_arg}
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
    h = {:meth => "allocate", :singleton => true}
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
    op = meth unless op
    def_method(meth, 1, "cond_binary", :op => op)
  end

  def cond_unary(meth)
    def_method(meth, 0, "cond_unary")
  end

  def bit_count(meth)
    def_method(meth, -1, "bit_count")
  end

  def bit_reduce(meth, init_bit)
    h = {:init_bit=>init_bit}
    def_method(meth, -1, "bit_reduce", h)
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

  def def_mod_func(meth, n_arg, tmpl=nil, opts={})
    h = {:meth => meth, :n_arg => n_arg}
    h.merge!(opts)
    tmpl ||= meth
    ModuleFunction.new(self, tmpl, h)
  end

  def math(meth, n=1, tmpl=nil)
    h = {:mod_var => 'mTM'}
    if tmpl.nil?
      case n
      when 1
        tmpl = "unary_s"
      when 2
        tmpl = "binary_s"
      when 3
        tmpl = "ternary_s"
      else
        raise "invalid n=#{n}"
      end
    end
    def_mod_func(meth, n, tmpl, h)
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

  def store_bit(cname)
    Store.new(self,"store_bit",cname.downcase,nil,"numo_c"+cname,nil)
  end

  def store
    Function.new(self,"store","store")
  end

  def find_method(meth)
    Function::DEFS.find{|x| x.kind_of?(Function) and meth == x.meth }
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
    load_type(type_file) if type_file
    dirs = template_dir || ["tmpl"]
    @tmpl_dirs = dirs.map{|d| File.join(File.dirname(erb_path),d)}
  end

  attr_reader :tmpl_dirs

  def load_type(file)
    eval File.read(file)
  end

  attrs = %w[
    class_name
    ctype
    template_dir
    blas_char
    complex_class_name
    complex_type
    real_class_name
    real_ctype
    has_math
    is_bit
    is_int
    is_unsigned
    is_float
    is_real
    is_complex
    is_object
    is_comparable
    is_double_precision
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
      @upcast << "#ifdef RUBY_INTEGER_UNIFICATION"
      @upcast << "rb_hash_aset(hCast, rb_cInteger, #{t});"
      @upcast << "#else"
      @upcast << "rb_hash_aset(hCast, rb_cFixnum, #{t});"
      @upcast << "rb_hash_aset(hCast, rb_cBignum, #{t});"
      @upcast << "#endif"
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

  def condition(klass)
    "#{klass}==#{tpclass}"
  end

  def extract_data(ptr,pos,x)
    case tpname
    when "bit"
      "{BIT_DIGIT b; LOAD_BIT(#{ptr},#{pos},b); x = m_from_real(b);}"
    when "robject"
      "#{x} = m_num_to_data(*(#{dtype}*)(#{ptr}+#{pos}))"
    when /complex/
      "{#{dtype} *p = (#{dtype}*)(#{ptr}+#{pos}); #{x} = c_new(REAL(*p),IMAG(*p));}"
    else
      "#{x} = m_from_real(*(#{dtype}*)(#{ptr}+#{pos}))"
    end
  end

  def self.definitions
    a = []
    DEFS.each do |x|
      if x.condition("")
        if x.tpname == x.parents[0].class_name.downcase
          a.unshift(x)
        else
          a.push(x)
        end
      end
    end
    a
  end
end

class StoreNum < Store
  def initialize(parent,tmpl)
    super(parent,tmpl,"numeric",nil,nil,nil)
  end

  def condition(klass)
    "IS_INTEGER_CLASS(#{klass}) || #{klass}==rb_cFloat || #{klass}==rb_cComplex"
  end
end

class StoreArray < Store
  def initialize(parent,tmpl)
    super(parent,tmpl,"array",nil,nil,nil)
  end

  def c_func
    "numo_#{tp}_#{tmpl}"
  end

  def condition(klass)
    "#{klass}==rb_cArray"
  end
end

class CastArray < StoreArray
  def condition(klass)
    nil
  end

  def c_func
    "numo_#{tp}_cast_#{tpname}"
  end

  def c_iter
    "iter_#{tp}_cast_#{tpname}"
  end
end
