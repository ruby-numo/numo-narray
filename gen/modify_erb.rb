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
      eval(src, b, (@filename || '(erb)'), 0)
    end
  end

  alias src_orig src

  def src
    src_with_cpp_line
  end

  def src_with_cpp_line
    linebreak = false
    @src.each_line.with_index.map do |line, num|
      if num==1
        s = "#line #{num} \"#{@filename}\"\n".dump
        line.sub!(/_erbout = '';/, "_erbout = #{s};")
      elsif /^;/ =~ line
        if linebreak
          s = "#line #{num} \"#{@filename}\"\n".dump
          line = ";_erbout.concat(#{s})"+line
          linebreak = false
        end
      elsif num > 1
        linebreak = true
      end
      line
    end.join
  end

end
