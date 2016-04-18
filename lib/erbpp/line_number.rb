class CountLnString < String

  def initialize(filename)
    @filename = filename
    @lnchar = "\n"
    @buf = ""
    @str = ""
    @countln = 1
    @current = 1
    super(report_line)
  end

  def report_line
    "#line #{@current} \"#{@filename}\"\n"
  end

  def concat0(s)
    ln(caller[0])
    @buf.concat(s)
    @str.concat(s)
  end

  def concat1(s)
    ln(caller[0])
    @buf.concat(s)
  end

  def ln(status=nil)
    case status
    when /:(\d+):/
      n = $1.to_i
    else
      n = status.to_i
    end
    return if n == @current
    if @current != @countln || @postpone
      if /^\s*$/ =~ @str || /\A#line / =~ @buf
        @postpone = true
      else
        concat(report_line)
        @postpone = false
      end
    end
    concat(@buf)
    @countln = @current + @buf.count(@lnchar)
    @current = n
    @buf = ""
    @str = ""
  end
end

class ERB
  alias result_orig result

  def result(b=new_toplevel)
    src = src_with_cpp_line
    if @safe_level
      proc {
        $SAFE = @safe_level
        eval(src, b, (@filename || '(erb)'), 0)
      }.call
    else
      #open("tmpout","w"){|f| f.write src} if /dtype/=~@filename
      eval(src, b, (@filename || '(erb)'), 0)
    end
  end

  alias src_orig src

  def src
    src_with_cpp_line
  end

  def src_with_cpp_line
    @src.each_line.with_index.map do |line, num|
      line.gsub!(/_erbout.concat "/,'_erbout.concat0 "')
      line.gsub!(/_erbout.concat\(/,'_erbout.concat1(')
      if num==0
        # skip
      elsif num==1
        f = @filename.dump
        line.sub!(/_erbout = '';/, "_erbout = CountLnString.new(#{f});")
      elsif /^; _erbout\.force_encoding/ =~ line
        line.sub!(/^;/,";_erbout.ln(#{num});")
      end
      line
    end.join
  end

end
