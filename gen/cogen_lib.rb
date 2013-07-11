require "erb"

TMPL_DIR="template"

module PutERB
  def erb
    file = TMPL_DIR+"/#{@tmpl}.c"
    if $embed
      "\n"+ERB.new(File.read(file)).result(binding)
    else
      puts "\n/* ERB from #{file} */"
      ERB.new(File.read(file)).run(binding)
    end
  end
end

class Cast
  include PutERB
  INIT = []

  def self.c_instance_method
    "nary_#{tp}_s_cast"
  end

  def initialize(tmpl,tpname,dtype,tpclass,macro)
    @tmpl=tmpl
    @tpname=tpname
    @dtype=dtype
    @tpclass=tpclass
    @macro=macro
  end
  attr_reader :tmpl, :tpname, :dtype, :tpclass, :macro

  def c_function
    "nary_#{tp}_cast_#{tpname}"
  end

  def c_iterator
    "iter_#{tp}_cast_#{tpname}"
  end

  def result
    INIT << self
    erb
  end

  def condition
    "rb_obj_is_kind_of(obj,#{tpclass})"
  end
end

class CastNum < Cast
  def initialize(tmpl)
    @tmpl=tmpl
    @tpname="numeric"
  end
  def condition
    "FIXNUM_P(obj) || TYPE(obj)==T_FLOAT || TYPE(obj)==T_BIGNUM || rb_obj_is_kind_of(obj,cComplex)"
  end
end

class CastArray < Cast
  def initialize(tmpl)
    @tmpl=tmpl
    @tpname="array"
  end
  def condition
    "TYPE(obj)==T_ARRAY"
  end
end

# ----------------------------------------------------------------------

class Template
  include PutERB
  INIT = []
  OPMAP = {
    "add"=>"+", "sub"=>"-", "mul"=>"*", "div"=>"/",
    "mod"=>"%", "pow"=>"**", "minus"=>"-@", "plus"=>"+@",
    "and"=>"&",
    "or"=>"|",
    "xor"=>"^",
    "not"=>"~@"
  }

  def self.alias(dst,src)
    INIT << "rb_define_alias(cT, \"#{dst}\", \"#{src}\");"
  end

  def initialize(tmpl,op,hash={})
    @tmpl=tmpl
    @op=op
    hash.each do |k,v|
      name = k.to_s
      ivar = "@"+name
      instance_variable_set(ivar,v)
      define_singleton_method(name){instance_variable_get(ivar)}
    end
  end
  attr_reader :op

  def c_instance_method
    "nary_#{tp}_#{op}"
  end

  def c_singleton_method
    "nary_#{tp}_s_#{op}"
  end

  def c_iterator
    "iter_#{tp}_#{op}"
  end

  def rb_define_singleton_method(n)
    "rb_define_singleton_method(cT, \"#{op}\", #{c_singleton_method}, #{n});"
  end

  def rb_define_method(n)
    "rb_define_method(cT, \"#{op_map}\", #{c_instance_method}, #{n});"
  end

  def rb_define_math(n)
    "rb_define_singleton_method(mTM, \"#{op}\", #{c_singleton_method}, #{n});"
  end

  def op_map
    OPMAP[op] || op
  end

  def def_singleton(n=0)
    INIT << rb_define_singleton_method(n)
    erb
  end

  def def_binary
    INIT << rb_define_singleton_method(2)
    INIT << rb_define_method(1)
    erb
  end

  def def_method(n=0)
    INIT << rb_define_method(n)
    erb
  end

  def def_func
    erb
  end

  def def_math(n=1)
    INIT << rb_define_math(n)
    erb
  end
end

# ----------------------------------------------------------------------

class Cogen

  def initialize
    @class_alias = []
    @upcast = []
  end

  attrs = %w[
    class_name
    class_alias
    ctype
    type_var
    type_name

    math_var

    real_class_name
    real_ctype
    real_type_var
    real_type_name

    has_math
    is_bit
    is_int
    is_float
    is_real
    is_complex
    is_object
    is_comparable
  ]
  attrs.each do |attr|
    ivar = ("@"+attr).to_sym
    define_method(attr){|*a| attr_def(ivar,*a)}
  end

  def attr_def(ivar,arg=nil)
    if arg.nil?
      instance_variable_get(ivar)
    else
      instance_variable_set(ivar,arg)
    end
  end

  alias tp type_name

  def define_type(name,ctype)
    @class_name = name
    @type_name = name.downcase
    @ctype = ctype
    @type_var = "c"+name
    @math_var = "m"+name+"Math"
  end

  def define_real(name,ctype)
    @real_class_name = name
    @real_ctype = ctype
    @real_type_var = "c"+name
    @real_type_name = name.downcase
  end

  def define_alias(*names)
    @class_alias.concat(names)
  end

  def upcast(c=nil,t="T")
    if c
      @upcast << "rb_hash_aset(hCast, c#{c}, c#{t});"
    else
      @upcast
    end
  end

  def def_singleton(ope,n=0)
    Template.new(ope,ope).def_singleton(n)
  end

  def def_method(tmpl,n=0)
    Template.new(tmpl,tmpl).def_method(n)
  end

  def binary(ope)
    Template.new("binary",ope).def_binary
  end

  def pow
    Template.new("pow","pow").def_binary
  end

  def unary(ope)
    Template.new("unary",ope).def_method
  end

  def unary2(ope,dtype,tpclass)
    Template.new("unary2",ope,
                 :dtype=>dtype,
                 :tpclass=>tpclass).def_method
  end

  def set2(ope,dtype,tpclass)
    Template.new("set2",ope,
                 :dtype=>dtype,
                 :tpclass=>tpclass).def_method(1)
  end

  def cond_binary(ope)
    Template.new("cond_binary",ope).def_binary
  end

  def cond_unary(ope)
    Template.new("cond_unary",ope).def_method
  end

  def bit_binary(ope)
    Template.new("bit_binary",ope).def_method(1)
  end

  def bit_unary(ope)
    Template.new("bit_unary",ope).def_method
  end

  def bit_count(ope)
    Template.new("bit_count",ope).def_method(-1)
  end

  def accum(ope)
    Template.new("accum",ope).def_method(-1)
  end

  def qsort(tp,dtype,dcast)
    Template.new("qsort",nil,
                 :tp=>tp,
                 :dtype=>dtype,
                 :dcast=>dcast).def_func
  end

  def def_func(ope,tmpl)
    Template.new(tmpl,ope).def_func
  end

  def def_alias(dst,src)
    Template.alias(dst,src)
  end


  def put_head
    Template.new("head",nil).erb
  end

  def put_init
    Template.new("init",nil).erb
  end

  def math(ope,n=1)
    case n
    when 1
      Template.new("unary_s",ope).def_math(n)
    when 2
      Template.new("binary_s",ope).def_math(n)
    when 3
      Template.new("ternary_s",ope).def_math(n)
    else
      raise "invalid n=#{n}"
    end
  end

  def cast_numeric
    CastNum.new("cast_numeric").result
  end

  def cast_array
    CastArray.new("cast_array").result
  end

  def cast_from(cname,dtype,macro)
    if type_name != cname.downcase
      Cast.new("cast_from",cname.downcase,dtype,"c"+cname,macro).result
    end
  end

  #def cast_from_real(cname,dtype)
  #  Cast.new("cast_from_real",cname.downcase,dtype,"c"+cname).result
  #end
end

# ----------------------------------------------------------------------

module Delegate
  @@cogen = Cogen.new
  module_function
  alias method_missing_alias method_missing
  def method_missing(method, *args, &block)
    if @@cogen.respond_to? method
      @@cogen.send(method, *args, &block)
    else
      method_missing_alias(method, *args)
    end
  end
end

include Delegate
