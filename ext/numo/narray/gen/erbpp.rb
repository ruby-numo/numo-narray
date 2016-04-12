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
