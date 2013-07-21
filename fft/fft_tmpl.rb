module Delegate
  module_function
  alias method_missing_alias method_missing
  def method_missing(method, *args, &block)
    if $params.kind_of?(Hash) and param=$params[method]
      ERB.new(param).result(binding)
    elsif $params.respond_to? method
      param = $params.send(method, *args, &block)
      ERB.new(param).result(binding)
    else
      method_missing_alias(method, *args)
    end
  end
end

include Delegate

$defs = [
{
:name   => "cdft",
:n_a    => "n*2",
:n_t    => "0",
:n_ip   => "2+sqrt(n)",
:n_w    => "n/2",
:dtype  => "dcomplex",
:class_name => "DComplex",
:class_var  => "cDComplex",
:args   => "n*2, g->sign, (double*)a, g->ip, g->w",
:doc => "
  Complex DFT (Discrete Fourier Transform),
  Fast Version (Split-Radix).
  @overload <%=name%>(narray,[sign])
  @param [NArray::<%=class_name%>] narray
  @param [Numeric] sign  0 or 1 or -1.
  @return [NArray::<%=class_name%>]
",
},
{
:name   => "rdft",
:n_a    => "n",
:n_t    => "0",
:n_ip   => "2+sqrt(n/2)",
:n_w    => "n/2",
:dtype  => "double",
:class_name => "DFloat",
:class_var  => "cDFloat",
:args   => "n, g->sign, a, g->ip, g->w",
:doc => "
  Real DFT (Discrete Fourier Transform),
  Fast Version (Split-Radix).
  @overload <%=name%>(narray,[sign])
  @param [NArray::<%=class_name%>] narray
  @param [Numeric] sign  0 or 1 or -1.
  @return [NArray::<%=class_name%>]
",
},
{
:name   => "ddct",
:n_a    => "n",
:n_t    => "0",
:n_ip   => "2+sqrt(n/2)",
:n_w    => "n*5/4",
:dtype  => "double",
:class_name => "DFloat",
:class_var  => "cDFloat",
:args   => "n, g->sign, a, g->ip, g->w",
:doc => "
  DCT (Discrete Cosine Transform),
  Fast Version (Split-Radix).
  @overload <%=name%>(narray,[sign])
  @param [NArray::<%=class_name%>] narray
  @param [Numeric] sign  0 or 1 or -1.
  @return [NArray::<%=class_name%>]
",
},
{
:name   => "ddst",
:n_a    => "n",
:n_t    => "0",
:n_ip   => "2+sqrt(n/2)",
:n_w    => "n*5/4",
:dtype  => "double",
:class_name => "DFloat",
:class_var  => "cDFloat",
:args   => "n, g->sign, a, g->ip, g->w",
:doc => "
  DST (Discrete Sine Transform),
  Fast Version (Split-Radix).
  @overload <%=name%>(narray,[sign])
  @param [NArray::<%=class_name%>] narray
  @param [Numeric] sign  0 or 1 or -1.
  @return [NArray::<%=class_name%>]
",
},
{
:name   => "dfct",
:n_a    => "n",
:n_t    => "n/2",
:n_ip   => "2+sqrt(n/4)",
:n_w    => "n*5/8",
:dtype  => "double",
:class_name => "DFloat",
:class_var  => "cDFloat",
:args   => "n, a, g->t, g->ip, g->w",
:doc => "
  Cosine Transform of RDFT (Real Symmetric DFT),
  Fast Version (Split-Radix).
  @overload <%=name%>(narray)
  @param [NArray::<%=class_name%>] narray
  @return [NArray::<%=class_name%>]
",
},
{
:name   => "dfst",
:n_a    => "n",
:n_t    => "n/2",
:n_ip   => "2+sqrt(n/4)",
:n_w    => "n*5/8",
:dtype  => "double",
:class_name => "DFloat",
:class_var  => "cDFloat",
:args   => "n, a, g->t, g->ip, g->w",
:doc => "
  Sine Transform of RDFT (Real Anti-symmetric DFT),
  Fast Version (Split-Radix).
  @overload <%=name%>(narray)
  @param [NArray::<%=class_name%>] narray
  @return [NArray::<%=class_name%>]
",
}
]
