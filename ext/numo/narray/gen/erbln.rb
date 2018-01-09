require 'erb'

class ErbPP

  class CountLnString < String

    def initialize(filename)
      @filename = filename
      @lnchar = "\n"
      @buf = ""
      @str = ""
      @countln = 1
      @current = 1
      super("\n"+report_line)
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
        if /\A\s*\z/ =~ @str || /\A#line / =~ @buf
          @postpone = true
        elsif @in_comment
          @postpone = false
        else
          if self[-1] != "\n"
            concat("\n")
          end
          concat(report_line)
          @postpone = false
        end
      end
      concat(@buf)

      b = @buf.gsub(/".*?(?<!\\)"/,'""')
      /^.*(\/\*)(.*?)$/ =~ b
      x = $2
      /^.*(\*\/)(.*?)$/ =~ b
      y = $2
      if x
        if y
          if x.size < y.size
            #:in_comment
            @in_comment = true
          else
            #:out_comment
            @in_comment = false
          end
        else
          #:in_comment
          @in_comment = true
        end
      else
        if y
          #:out_comment
          @in_comment = false
        else
          #:keep
        end
      end

      @countln = @current + @buf.count(@lnchar)
      @current = n
      @buf = ""
      @str = ""
    end

    def d(s)
      p [s, [x,y], r]
      r
    end

    def final
      ln(caller[0])
      concat(@buf)
    end

  end

  class ERBLN

    def initialize(str, filename, trim_mode=nil, eoutvar='_erbout')
      @filename = filename
      compiler = ERB::Compiler.new(trim_mode)
      set_eoutvar(compiler, eoutvar)
      @src, @enc = *compiler.compile(str)
    end

    def set_eoutvar(compiler, eoutvar = '_erbout')
      compiler.put_cmd = "#{eoutvar}.concat0"
      compiler.insert_cmd = "#{eoutvar}.concat1"
      compiler.pre_cmd = ["#{eoutvar} = CountLnString.new(#{@filename.inspect})"]
      compiler.post_cmd = ["#{eoutvar}.final"]
    end

    def run(b)
      print self.result(b)
    end

    def result(b)
      #open("tmpout","a") do |f|
      #  f.puts "\n#file:#{@filename}"
      #  f.puts @src
      #end
      eval(@src, b, (@filename || '(erb)'), 0)
    end

    #require "fileutils"
    #FileUtils.rm_f("tmpout")
  end

end
