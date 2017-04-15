require_relative './erbpp2'

module NArrayMethod

  def binary(meth, ope=nil)
    ope = meth if ope.nil?
    def_method(meth, "binary", op:ope)
  end

  def binary2(meth, ope=nil)
    ope = meth if ope.nil?
    def_method(meth, "binary2", op:ope)
  end

  def unary(meth, ope=nil)
    def_method(meth, "unary", op:ope)
  end

  def pow
    def_method("pow", "pow", op:"**")
  end

  def unary2(meth, dtype, tpclass)
    h = {dtype:dtype, tpclass:tpclass}
    def_method(meth, "unary2", **h)
  end

  def set2(meth, dtype, tpclass)
    h = {dtype:dtype, tpclass:tpclass}
    def_method(meth, "set2", h)
  end

  def cond_binary(meth,op=nil)
    op = meth unless op
    def_method(meth, "cond_binary", op:op)
  end

  def cond_unary(meth)
    def_method(meth, "cond_unary")
  end

  def bit_count(meth)
    def_method(meth, "bit_count")
  end

  def bit_reduce(meth, init_bit)
    h = {init_bit:init_bit}
    def_method(meth, "bit_reduce", **h)
  end

  def accum(meth, dtype, tpclass)
    h = {dtype:dtype, tpclass:tpclass}
    def_method(meth, "accum", **h)
  end

  def accum_index(meth)
    def_method(meth, "accum_index")
  end

  def cum(meth, cmacro)
    def_method(meth, "cum", cmacro:cmacro)
  end

  def accum_binary(meth, ope=nil)
    ope = meth if ope.nil?
    def_method(meth, "accum_binary", op:ope)
  end

  def qsort(type_name, dtype, dcast, suffix="")
    h = {type_name:type_name, dtype:dtype, dcast:dcast, suffix:suffix}
    def_method("qsort", **h)
  end

  def math(meth, n=1, tmpl=nil)
    h = {mod_var:'mTM'}
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
    def_module_function(meth, tmpl, **h)
  end

end

# ----------------------------------------------------------------------

module NArrayType

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
    @class_alias ||= []
    @class_alias.concat(args)
  end

  def upcast(c=nil,t=nil)
    @upcast ||= []
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
    @upcast ||= []
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

module StoreFrom

  def store_from(cname, dtype=nil, macro=nil)
    tmpl = (cname=="Bit") ? "store_bit" : "store_from"
    h = { name:cname.downcase,
          type_name:cname,
          type_var:"numo_c"+cname,
          dtype:dtype,
          macro:macro }
    Store.new(self, tmpl, **h)
  end

  def store_numeric
    StoreNum.new(self, "store_numeric", name:"numeric")
  end

  def store_array
    StoreArray.new(self, "store_array", name:"array")
  end

  def definitions
    a = []
    @children.each do |x|
      if x.condition("")
        if x.get(:type_name) == parent.class_name
          a.unshift(x)
        else
          a.push(x)
        end
      end
    end
    a
  end
end

# ----------------------------------------------------------------------

class Store < DefMethod
  def c_func(n=nil)
    "#{parent.parent.name}_store_#{name}"
  end

  def condition(klass)
    "#{klass}==#{type_var}"
  end

  def extract_data(ptr,pos,x)
    case type_name
    when "Bit"
      "{BIT_DIGIT b; LOAD_BIT(#{ptr},#{pos},b); x = m_from_real(b);}"
    when "RObject"
      "#{x} = m_num_to_data(*(#{dtype}*)(#{ptr}+#{pos}))"
    when /Complex/
      "{#{dtype} *p = (#{dtype}*)(#{ptr}+#{pos}); #{x} = c_new(REAL(*p),IMAG(*p));}"
    else
      "#{x} = m_from_real(*(#{dtype}*)(#{ptr}+#{pos}))"
    end
  end
end

class StoreNum < Store
  def condition(klass)
    "IS_INTEGER_CLASS(#{klass}) || #{klass}==rb_cFloat || #{klass}==rb_cComplex"
  end
end

class StoreArray < Store
  def condition(klass)
    "#{klass}==rb_cArray"
  end
end
