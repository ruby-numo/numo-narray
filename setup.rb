#!/usr/bin/env ruby
# Setup.rb v5.1.0
#
# This is a stand-alone bundle of the setup.rb application.
# You can place it in your projects script/ directory, or
# call it 'setup.rb' and place it in your project's
# root directory (just like old times).
#
# NOTE: As of version 5.1.0 this bundled rendition is also
# being used for the bin/setup.rb exe. Rather than the previous:
#
#   require 'setup/command'
#   Setup::Command.run
#
# By doing so, +rvm+ should be able to use it across all rubies
# without issue and without needing to install it for each.
require 'yaml'
module Setup
  VERSION = '5.2.0'  #:erb: VERSION = '<%= version %>'
end
class << File #:nodoc: all
  unless respond_to?(:read)   # Ruby 1.6 and less
    def read(fname)
      open(fname){ |f| return f.read }
    end
  end
  def dir?(path)
    directory?((path[-1,1] == '/') ? path : path + '/')
  end
end
unless Errno.const_defined?(:ENOTEMPTY)   # Windows?
  module Errno  #:nodoc:
    class ENOTEMPTY  #:nodoc:
    end
  end
end
module Setup
  META_EXTENSION_DIR = '.setup'
  FILETYPES = %w( bin lib ext data etc man doc )
  INSTALL_RECORD = 'SetupReceipt'
  CONFIG_FILE = 'SetupConfig'
end
module Setup
  class Project
    ROOT_MARKER = '{.index,setup.rb,.setup,lib/}'
    def initialize
      @dotindex_file = find('.index')
      @dotindex = YAML.load_file(@dotindex_file) if @dotindex_file
      @name     = nil
      @version  = nil
      @loadpath = ['lib']
      if @dotindex
        @name     = @dotindex['name']
        @version  = @dotindex['version']
        @loadpath = (@dotindex['paths'] || {})['load']
      else
        if file = find('.setup/name')
          @name = File.read(file).strip
        end
        if file = find('.setup/version')
          @version = File.read(file).strip
        end
        if file = find('.setup/loadpath')
          @loadpath = File.read(file).strip
        end
      end
    end
    attr :dotindex
    attr :name
    attr :version
    attr :loadpath
    alias load_path loadpath
    def rootdir
      @rootdir ||= (
        root = Dir.glob(File.join(Dir.pwd, ROOT_MARKER), File::FNM_CASEFOLD).first
        if !root
          raise Error, "not a project directory"
        else
          Dir.pwd
        end
      )
    end
    def extconfs
      @extconfs ||= Dir['ext/**/extconf.rb']
    end
    def extensions
      @extensions ||= extconfs.collect{ |f| File.dirname(f) }
    end
    def compiles?
      !extensions.empty?
    end
    def yardopts
      Dir.glob(File.join(rootdir, '.yardopts')).first
    end
    def document
      Dir.glob(File.join(rootdir, '.document')).first
    end
    def find(glob, flags=0)
      case flags
      when :casefold
        flags = File::FNM_CASEFOLD
      else
        flags = flags.to_i
      end      
      Dir.glob(File.join(rootdir, glob), flags).first
    end
  end
end
module Setup
  class Session
    attr :options
    def initialize(options={})
      @options = options
      self.io ||= StringIO.new  # log instead ?
    end
    def io
      @options[:io]
    end
    def io=(anyio)
      @options[:io] = anyio
    end
    def trace?; @options[:trace]; end
    def trace=(val)
      @options[:trace] = val
    end
    def trial?; @options[:trial]; end
    alias_method :dryrun?, :trial?
    def trial=(val)
      @options[:trial] = val
    end
    alias_method :dryrun=, :trial=
    def quiet?; @options[:quiet]; end
    def quiet=(val)
      @options[:quiet] = val
    end
    def force?; @options[:force]; end
    def force=(val)
      @options[:force] = val
    end
    def compile?
      configuration.compile? && project.compiles?
    end
    def all
        config
        compile
      if configuration.test?
        ok = test
        exit 1 unless ok
      end
      install
    end
    def config
      log_header('Preconfig')
      if configuration.save_config
        io.print "#{CONFIG_FILE} was saved. " unless quiet?
      else
        io.print "#{CONFIG_FILE} is current. " unless quiet?
      end
      io.puts "Edit to customize configuration." unless quiet?
      puts configuration if trace? && !quiet?
    end
    def compile
      if compile?
        log_header('Compile')
        compiler.configure
        compiler.compile
      end
    end
    alias_method :make, :compile
    alias_method :setup, :make
    def install
      log_header('Install')
      installer.install
    end
    def test
      return true unless tester.testable?
      log_header('Test')
      tester.test
    end
    def clean
      log_header('Clean')
      compiler.clean
    end
    def distclean
      log_header('Distclean')
      compiler.distclean
    end
    def uninstall
      if !File.exist?(INSTALL_RECORD)
        io.puts "Nothing is installed."
        return
      end
      log_header('Uninstall')
      uninstaller.uninstall
      io.puts('Ok.')
    end
    def show
      puts configuration
    end
    def project
      @project ||= Project.new
    end
    def configuration
      @configuration ||= Configuration.new
    end
    def compiler
      @compiler ||= Compiler.new(project, configuration, options)
    end
    def installer
      @installer ||= Installer.new(project, configuration, options)
    end
    def tester
      @tester ||= Tester.new(project, configuration, options)
    end
    def uninstaller
      @uninstaller ||= Uninstaller.new(project, configuration, options)
    end
    def log_header(phase)
      return if quiet?
      if trial?
        str = "#{phase.upcase} (trail run)"
      else
        str = "#{phase.upcase}"
      end
      line = "- " * 35
      line[0..str.size+3] = str
      io.puts("\n- - #{line}\n\n")
    end
  end
end
module Setup
  class Base
    attr :project
    attr :config
    attr_accessor :trial
    attr_accessor :trace
    attr_accessor :quiet
    attr_accessor :force
    attr_accessor :io
    def initialize(project, configuration, options={})
      @project = project
      @config  = configuration
      initialize_hooks
      options.each do |k,v|
        __send__("#{k}=", v) if respond_to?("#{k}=")
      end
    end
    def initialize_hooks
      file = META_EXTENSION_DIR + "/#{self.class.name.downcase}.rb"
      if File.exist?(file)
        script = File.read(file)
        (class << self; self; end).class_eval(script)
      end
    end
    def trial? ; @trial ; end
    def trace? ; @trace ; end
    def quiet? ; @quiet ; end
    def force? ; @force ; end
    def rootdir
      project.rootdir
    end
    def bash(*args)
      $stderr.puts args.join(' ') if trace?
      system(*args) or raise RuntimeError, "system(#{args.map{|a| a.inspect }.join(' ')}) failed"
    end
    alias_method :command, :bash
    def ruby(*args)
      bash(config.rubyprog, *args)
    end
    def trace_off #:yield:
      begin
        save, @trace = trace?, false
        yield
      ensure
        @trace = save
      end
    end
    def rm_f(path)
      io.puts "rm -f #{path}" if trace? or trial?
      return if trial?
      force_remove_file(path)
    end
    def force_remove_file(path)
      begin
        remove_file(path)
      rescue
      end
    end
    def remove_file(path)
      File.chmod 0777, path
      File.unlink(path)
    end
    def rmdir(path)
      io.puts "rmdir #{path}" if trace? or trial?
      return if trial?
      Dir.rmdir(path)
    end
  end
  class Error < StandardError
  end
end
module Setup
  class Compiler < Base
    def compiles?
      !extdirs.empty?
    end
    def configure
      extdirs.each do |dir|
        Dir.chdir(dir) do
          if File.exist?('extconf.rb') && !FileUtils.uptodate?('Makefile', ['extconf.rb'])
            ruby("extconf.rb")
          end
        end
      end
    end
    def compile
      extdirs.each do |dir|
        Dir.chdir(dir) do
          make
        end
      end
    end
    def clean
      extdirs.each do |dir|
        Dir.chdir(dir) do
          make('clean')
        end
      end
    end
    def distclean
      extdirs.each do |dir|
        Dir.chdir(dir) do
          make('distclean')
        end
      end
    end
    def extdirs
      Dir['ext/**/*/{MANIFEST,extconf.rb}'].map do |f|
        File.dirname(f)
      end.uniq
    end
    def make(task=nil)
      return unless File.exist?('Makefile')
      bash(*[config.makeprog, task].compact)
    end
  end
end
require 'rbconfig'
require 'fileutils'
require 'erb'
require 'yaml'
require 'shellwords'
module Setup
  class Configuration
    RBCONFIG  = ::RbConfig::CONFIG
    META_CONFIG_FILE = META_EXTENSION_DIR + '/metaconfig.rb'
    def self.options
      @@options ||= []
    end
    def self.option(name, *args) #type, description)
      options << [name.to_s, *args] #type, description]
      attr_accessor(name)
    end
    option :prefix          , :path, 'path prefix of target environment'
    option :bindir          , :path, 'directory for commands'
    option :libdir          , :path, 'directory for libraries'
    option :datadir         , :path, 'directory for shared data'
    option :mandir          , :path, 'directory for man pages'
    option :docdir          , :path, 'directory for documentation'
    option :rbdir           , :path, 'directory for ruby scripts'
    option :sodir           , :path, 'directory for ruby extentions'
    option :sysconfdir      , :path, 'directory for system configuration files'
    option :localstatedir   , :path, 'directory for local state data'
    option :libruby         , :path, 'directory for ruby libraries'
    option :librubyver      , :path, 'directory for standard ruby libraries'
    option :librubyverarch  , :path, 'directory for standard ruby extensions'
    option :siteruby        , :path, 'directory for version-independent aux ruby libraries'
    option :siterubyver     , :path, 'directory for aux ruby libraries'
    option :siterubyverarch , :path, 'directory for aux ruby binaries'
    option :rubypath        , :prog, 'path to set to #! line'
    option :rubyprog        , :prog, 'ruby program used for installation'
    option :makeprog        , :prog, 'make program to compile ruby extentions'
    option :extconfopt      , :opts, 'options to pass-thru to extconf.rb'
    option :shebang         , :pick, 'shebang line (#!) editing mode (all,ruby,never)'
    option :no_test, :t     , :bool, 'run pre-installation tests'
    option :no_doc          , :bool, 'install doc/ directory'
    option :no_ext          , :bool, 'compile/install ruby extentions'
    option :install_prefix  , :path, 'install to alternate root location'
    option :root            , :path, 'install to alternate root location'
    option :installdirs     , :pick, 'install location mode (site,std,home)'  #, local)
    option :type            , :pick, 'install location mode (site,std,home)'
    ::RbConfig::CONFIG.each do |key,val|
      next if key == "configure_args"
      name = key.to_s.downcase
      define_method(name){ val }
    end
    config_args = Shellwords.shellwords(::RbConfig::CONFIG["configure_args"])
    config_args.each do |ent|
      if ent.index("=")
        key, val = *ent.split("=")
      else
        key, val = ent, true
      end
      name = key.downcase
      name = name.sub(/^--/,'')
      name = name.gsub(/-/,'_')
      define_method(name){ val }
    end
    def options
      self.class.options
    end
    def initialize(values={})
      initialize_metaconfig
      initialize_defaults
      initialize_environment
      initialize_configfile unless values[:reset]
      values.each{ |k,v| __send__("#{k}=", v) }
      yield(self) if block_given?
    end
    def initialize_metaconfig
      if File.exist?(META_CONFIG_FILE)
        script = File.read(META_CONFIG_FILE)
        (class << self; self; end).class_eval(script)
      end
    end
    def initialize_defaults
      self.type    = 'site'
      self.no_ri   = true
      self.no_test = true
      self.no_doc  = true
      self.no_ext  = false
    end
    def initialize_environment
      options.each do |name, *args|
        if value = ENV["RUBYSETUP_#{name.to_s.upcase}"]
          __send__("#{name}=", value)
        end
      end
    end
    def initialize_configfile
      if exist?
        erb = ERB.new(File.read(CONFIG_FILE))
        txt = erb.result(binding)
        dat = YAML.load(txt)
        dat.each do |k, v|
          next if 'type' == k
          next if 'installdirs' == k
          k = k.gsub('-','_')
          __send__("#{k}=", v)
        end
        if dat['type']
          self.type = dat['type']
        end
        if dat['installdirs']
          self.installdirs = dat['installdirs']
        end
      end
    end
    attr_accessor :reset
    def base_bindir
      @base_bindir ||= subprefix('bindir')
    end
    def base_libdir
      @base_libdir ||= subprefix('libdir')
    end
    def base_datadir
      @base_datadir ||= subprefix('datadir')
    end
    def base_mandir
      @base_mandir ||= subprefix('mandir')
    end
    def base_docdir
      @base_docdir || File.dirname(subprefix('docdir'))
    end
    def base_rubylibdir
      @rubylibdir ||= subprefix('rubylibdir')
    end
    def base_rubyarchdir
      @base_rubyarchdir ||= subprefix('archdir')
    end
    def base_sysconfdir
      @base_sysconfdir ||= subprefix('sysconfdir')
    end
    def base_localstatedir
      @base_localstatedir ||= subprefix('localstatedir')
    end
    def type
      @type ||= 'site'
    end
    def type=(val)
      @type = val
      case val.to_s
      when 'std', 'ruby'
        @rbdir = librubyver       #'$librubyver'
        @sodir = librubyverarch   #'$librubyverarch'
      when 'site'
        @rbdir = siterubyver      #'$siterubyver'
        @sodir = siterubyverarch  #'$siterubyverarch'
      when 'home'
        self.prefix = File.join(home, '.local')  # TODO: Use XDG
        @rbdir = nil #'$libdir/ruby'
        @sodir = nil #'$libdir/ruby'
      else
        raise Error, "bad config: use type=(std|site|home) [#{val}]"
      end
    end
    alias_method :installdirs, :type
    alias_method :installdirs=, :type=
    alias_method :install_prefix, :root
    alias_method :install_prefix=, :root=
    def prefix
      @prefix ||= RBCONFIG['prefix']
    end
    def prefix=(path)
      @prefix = pathname(path)
    end
    def libruby
      @libruby ||= RBCONFIG['prefix'] + "/lib/ruby"
    end
    def libruby=(path)
      path = pathname(path)
      @librubyver = librubyver.sub(libruby, path)
      @librubyverarch = librubyverarch.sub(libruby, path)
      @libruby = path
    end
    def librubyver
      @librubyver ||= RBCONFIG['rubylibdir']
    end
    def librubyver=(path)
      @librubyver = pathname(path)
    end
    def librubyverarch
      @librubyverarch ||= RBCONFIG['archdir']
    end
    def librubyverarch=(path)
      @librubyverarch = pathname(path)
    end
    def siteruby
      @siteruby ||= RBCONFIG['sitedir']
    end
    def siteruby=(path)
      path = pathname(path)
      @siterubyver = siterubyver.sub(siteruby, path)
      @siterubyverarch = siterubyverarch.sub(siteruby, path)
      @siteruby = path
    end
    def siterubyver
      @siterubyver ||= RBCONFIG['sitelibdir']
    end
    def siterubyver=(path)
      @siterubyver = pathname(path)
    end
    def siterubyverarch
      @siterubyverarch ||= RBCONFIG['sitearchdir']
    end
    def siterubyverarch=(path)
      @siterubyverarch = pathname(path)
    end
    def bindir
      @bindir || File.join(prefix, base_bindir)
    end
    def bindir=(path)
      @bindir = pathname(path)
    end
    def libdir
      @libdir || File.join(prefix, base_libdir)
    end
    def libdir=(path)
      @libdir = pathname(path)
    end
    def datadir
      @datadir || File.join(prefix, base_datadir)
    end
    def datadir=(path)
      @datadir = pathname(path)
    end
    def mandir
      @mandir || File.join(prefix,  base_mandir)
    end
    def mandir=(path)
      @mandir = pathname(path)
    end
    def docdir
      @docdir || File.join(prefix, base_docdir)
    end
    def docdir=(path)
      @docdir = pathname(path)
    end
    def rbdir
      @rbdir || File.join(prefix, base_rubylibdir)
    end
    def sodir
      @sodir || File.join(prefix, base_rubyarchdir)
    end
    def sysconfdir
      @sysconfdir ||= base_sysconfdir
    end
    def sysconfdir=(path)
      @sysconfdir = pathname(path)
    end
    def localstatedir
      @localstatedir ||= base_localstatedir
    end
    def localstatedir=(path)
      @localstatedir = pathname(path)
    end
    def rubypath
      @rubypath ||= File.join(RBCONFIG['bindir'], RBCONFIG['ruby_install_name'] + RBCONFIG['EXEEXT'])
    end
    def rubypath=(path)
      @rubypath = pathname(path)
    end
    def rubyprog
      @rubyprog || rubypath
    end
    def rubyprog=(command)
      @rubyprog = command
    end
    def makeprog
      @makeprog ||= (
        if arg = RBCONFIG['configure_args'].split.detect {|arg| /--with-make-prog=/ =~ arg }
          arg.sub(/'/, '').split(/=/, 2)[1]
        else
          'make'
        end
      )
    end
    def makeprog=(command)
      @makeprog = command
    end
    def extconfopt
      @extconfopt ||= ''
    end
    def extconfopt=(string)
      @extconfopt = string
    end
    def shebang
      @shebang ||= 'ruby'
    end
    def shebang=(val)
      if %w(all ruby never).include?(val)
        @shebang = val
      else
        raise Error, "bad config: use SHEBANG=(all|ruby|never) [#{val}]"
      end
    end
    def no_ext
      @no_ext
    end
    def no_ext=(val)
      @no_ext = boolean(val)
    end
    def no_test
      @no_test
    end
    def no_test=(val)
      @no_test = boolean(val)
    end
    def no_doc
      @no_doc
    end
    def no_doc=(val)
      @no_doc = boolean(val)
    end
    def no_ri
      @no_ri
    end
    def no_ri=(val)
      @no_ri = boolean(val)
    end
    def compile?
      !no_ext
    end
    def test?
      !no_test
    end
    def doc?
      !no_doc
    end
    def to_h
      h = {}
      options.each do |name, *args|
        h[name.to_s] = __send__(name)
      end
      h
    end
    def to_s
      to_yaml.sub(/\A---\s*\n/,'')
    end
    def to_yaml(*args)
      to_h.to_yaml(*args)
    end
    def save_config
      out = to_yaml
      if not File.exist?(File.dirname(CONFIG_FILE))
        FileUtils.mkdir_p(File.dirname(CONFIG_FILE))
      end
      if File.exist?(CONFIG_FILE)
        txt = File.read(CONFIG_FILE)
        return nil if txt == out
      end          
      File.open(CONFIG_FILE, 'w'){ |f| f << out }
      true
    end
    def exist?
      File.exist?(CONFIG_FILE)
    end
  private
    def pathname(path)
      path.gsub(%r<\\$([^/]+)>){ self[$1] }
    end
    def boolean(val, name=nil)
      case val
      when true, false, nil
        val
      else
        case val.to_s.downcase
        when 'y', 'yes', 't', 'true'
           true
        when 'n', 'no', 'f', 'false'
           false
        else
          raise Error, "bad config: use --#{name}=(yes|no) [\#{val}]"
        end
      end
    end
    def subprefix(path, with='')
      val = RBCONFIG[path]
      raise "Unknown path -- #{path}" if val.nil?
      prefix = Regexp.quote(RBCONFIG['prefix'])
      val.sub(/\A#{prefix}/, with)
    end
    def home
      ENV['HOME'] || raise(Error, 'HOME is not set.')
    end
  end #class ConfigTable
end #module Setup
=begin
    def inintialize_metaconfig
      path = Dir.glob(METACONFIG_FILE).first
      if path && File.file?(path)
        MetaConfigEnvironment.new(self).instance_eval(File.read(path), path)
      end
    end
    class MetaConfigEnvironment
      def initialize(config) #, installer)
        @config    = config
      end
      def config_names
        @config.descriptions.collect{ |n, t, d| n.to_s }
      end
      def config?(name)
        @config.descriptions.find do |sym, type, desc|
          sym.to_s == name.to_s
        end
      end
      def bool_config?(name)
        @config.descriptions.find do |sym, type, desc|
          sym.to_s == name.to_s && type == :bool
        end
      end
      def path_config?(name)
        @config.descriptions.find do |sym, type, desc|
          sym.to_s == name.to_s && type == :path
        end
      end
      def value_config?(name)
        @config.descriptions.find do |sym, type, desc|
          sym.to_s == name.to_s && type != :prog
        end
      end
      def add_config(name, default, desc)
        @config.descriptions << [name.to_sym, nil, desc]
      end
      def add_bool_config(name, default, desc)
        @config.descriptions << [name.to_sym, :bool, desc]
      end
      def add_path_config(name, default, desc)
        @config.descriptions << [name.to_sym, :path, desc]
      end
      def set_config_default(name, default)
        @config[name] = default
      end
      def remove_config(name)
        item = @config.descriptions.find do |sym, type, desc|
          sym.to_s == name.to_s
        end
        index = @config.descriptions.index(item)
        @config.descriptions.delete(index)
      end
    end
=end
module Setup
  class Installer < Base
    def install_prefix
      config.install_prefix
    end
    def install
      Dir.chdir(rootdir) do
        install_bin
        install_ext
        install_lib
        install_data
        install_man
        install_doc
        install_etc
        prune_install_record
      end
    end
    def install_bin
      return unless directory?('bin')
      report_transfer('bin', config.bindir)
      files = files('bin')
      install_files('bin', files, config.bindir, 0755)
    end
    def install_ext
      return unless directory?('ext')
      report_transfer('ext', config.sodir)
      files = files('ext')
      files = select_dllext(files)
      files.each do |file|
        name = File.join(File.dirname(File.dirname(file)), File.basename(file))
        dest = destination(config.sodir, name)
        install_file('ext', file, dest, 0555, install_prefix)
      end
    end
    def install_lib
      return unless directory?('lib')
      report_transfer('lib', config.rbdir)
      files = files('lib')
      install_files('lib', files, config.rbdir, 0644)
    end
    def install_data
      return unless directory?('data')
      report_transfer('data', config.datadir)
      files = files('data')
      install_files('data', files, config.datadir, 0644)
    end
    def install_etc
      return unless directory?('etc')
      report_transfer('etc', config.sysconfdir)
      files = files('etc')
      install_files('etc', files, config.sysconfdir, 0644)
    end
    def install_man
      return unless directory?('man')
      report_transfer('man', config.mandir)
      files = files('man')
      install_files('man', files, config.mandir, 0644)
    end
    def install_doc
      return unless config.doc?
      return unless directory?('doc')
      return unless project.name
      dir = File.join(config.docdir, "ruby-#{project.name}")
      report_transfer('doc', dir)
      files = files('doc')
      install_files('doc', files, dir, 0644)
    end
  private
    def report_transfer(source, directory)
      unless quiet?
        if install_prefix
          out = File.join(install_prefix, directory)
        else
          out = directory
        end
        io.puts "* #{source} -> #{out}"
      end
    end
    def directory?(path)
      File.directory?(path)
    end
    def files(dir)
      files = Dir["#{dir}/**/*"]
      files = files.select{ |f| File.file?(f) }
      files = files.map{ |f| f.sub("#{dir}/", '') }
      files
    end
    def select_dllext(files)
      ents = files.select do |file| 
        File.extname(file) == ".#{dllext}"
      end
      if ents.empty? && !files.empty?
        raise Error, "ruby extention not compiled: 'setup.rb compile' first"
      end
      ents
    end
    def dllext
      config.dlext
    end
    def install_files(dir, list, dest, mode)
      list.each do |fname|
        rdest = destination(dest, fname)
        install_file(dir, fname, rdest, mode, install_prefix)
      end
    end
    def install_file(dir, from, dest, mode, prefix=nil)
      mkdir_p(File.dirname(dest))
      if trace? or trial?
        io.puts "install #{dir}/#{from} #{dest}"
      end
      return if trial?
      str = binread(File.join(dir, from))
      if diff?(str, dest)
        trace_off {
          rm_f(dest) if File.exist?(dest)
        }
        File.open(dest, 'wb'){ |f| f.write(str) }
        File.chmod(mode, dest)
      end
      record_installation(dest) # record file as installed
    end
    def mkdir_p(dirname) #, prefix=nil)
      return if File.directory?(dirname)
      io.puts "mkdir -p #{dirname}" if trace? or trial?
      return if trial?
      dirs = File.expand_path(dirname).split(%r<(?=/)>)
      if /\A[a-z]:\z/i =~ dirs[0]
        disk = dirs.shift
        dirs[0] = disk + dirs[0]
      end
      dirs.each_index do |idx|
        path = dirs[0..idx].join('')
        unless File.dir?(path)
          Dir.mkdir(path)
        end
        record_installation(path)  # record directories made
      end
    end
    def record_installation(path)
      File.open(install_record, 'a') do |f|
        f.puts(path)
      end
    end
    def prune_install_record
      entries = File.read(install_record).split("\n")
      entries.uniq!
      File.open(install_record, 'w') do |f|
        f << entries.join("\n")
        f << "\n"
      end
    end
    def install_record
      @install_record ||= (
        file = INSTALL_RECORD
        dir  = File.dirname(file)
        unless File.directory?(dir)
          FileUtils.mkdir_p(dir)
        end
        file
      )
    end
    def destination(dir, file)
      dest = install_prefix ? File.join(install_prefix, File.expand_path(dir)) : dir
      dest = File.join(dest, file) #if File.dir?(dest)
      dest = File.expand_path(dest)
      dest
    end
    def diff?(new_content, path)
      return true unless File.exist?(path)
      new_content != binread(path)
    end
    def binread(fname)
      File.open(fname, 'rb') do |f|
        return f.read
      end
    end
    def install_shebang(files, dir)
      files.each do |file|
        path = File.join(dir, File.basename(file))
        update_shebang_line(path)
      end
    end
    def update_shebang_line(path)
      return if trial?
      return if config.shebang == 'never'
      old = Shebang.load(path)
      if old
        if old.args.size > 1
          $stderr.puts "warning: #{path}"
          $stderr.puts "Shebang line has too many args."
          $stderr.puts "It is not portable and your program may not work."
        end
        new = new_shebang(old)
        return if new.to_s == old.to_s
      else
        return unless config.shebang == 'all'
        new = Shebang.new(config.rubypath)
      end
      $stderr.puts "updating shebang: #{File.basename(path)}" if trace?
      open_atomic_writer(path) do |output|
        File.open(path, 'rb') do |f|
          f.gets if old   # discard
          output.puts new.to_s
          output.print f.read
        end
      end
    end
    def new_shebang(old)
      if /\Aruby/ =~ File.basename(old.cmd)
        Shebang.new(config.rubypath, old.args)
      elsif File.basename(old.cmd) == 'env' and old.args.first == 'ruby'
        Shebang.new(config.rubypath, old.args[1..-1])
      else
        return old unless config.shebang == 'all'
        Shebang.new(config.rubypath)
      end
    end
    def open_atomic_writer(path, &block)
      tmpfile = File.basename(path) + '.tmp'
      begin
        File.open(tmpfile, 'wb', &block)
        File.rename tmpfile, File.basename(path)
      ensure
        File.unlink tmpfile if File.exist?(tmpfile)
      end
    end
    class Shebang
      def Shebang.load(path)
        line = nil
        File.open(path) {|f|
          line = f.gets
        }
        return nil unless /\A#!/ =~ line
        parse(line)
      end
      def Shebang.parse(line)
        cmd, *args = *line.strip.sub(/\A\#!/, '').split(' ')
        new(cmd, args)
      end
      def initialize(cmd, args = [])
        @cmd = cmd
        @args = args
      end
      attr_reader :cmd
      attr_reader :args
      def to_s
        "#! #{@cmd}" + (@args.empty? ? '' : " #{@args.join(' ')}")
      end
    end
  end
end
module Setup
  class Tester < Base
    RUBYSCRIPT  = META_EXTENSION_DIR + '/test.rb'
    SHELLSCRIPT = META_EXTENSION_DIR + '/test.sh'
    DEPRECATED_RUBYSCRIPT  = META_EXTENSION_DIR + '/testrc.rb'
    def testable?
      if File.exist?(DEPRECATED_RUBYSCRIPT)
        warn "Must use `.setup/test.rb' instead or `.setup/testrc.rb' to support testing."
      end
      return false if config.no_test
      return true  if File.exist?(RUBYSCRIPT)
      return true  if File.exist?(SHELLSCRIPT)
      false
    end
    def test
      return true unless testable?
      if File.exist?(RUBYSCRIPT)
        test_rubyscript
      elsif File.exist?(SHELLSCRIPT)
        test_shellscript
      else
        true
      end
    end
    def test_shellscript
      bash(SHELLSCRIPT)
    end
    def test_rubyscript
      ruby(RUBYSCRIPT)
    end
  end
end
module Setup
  class Uninstaller < Base
    def uninstall
      return unless File.exist?(INSTALL_RECORD)
      files = []
      dirs  = []
      paths.each do |path|
        dirs  << path if File.dir?(path)
        files << path if File.file?(path)
      end
      if dirs.empty? && files.empty?
        io.outs "Nothing to remove."
        return
      end
      files.sort!{ |a,b| b.size <=> a.size }
      dirs.sort!{ |a,b| b.size <=> a.size }
      if !force? && !trial?
        puts (files + dirs).collect{ |f| "#{f}" }.join("\n")
        puts
        puts "Must use --force option to remove these files and directories that become empty."
        return
      end
      files.each do |file|
        rm_f(file)
      end
      dirs.each do |dir|
        entries = Dir.entries(dir)
        entries.delete('.')
        entries.delete('..')
          rmdir(dir) if entries.empty?
      end
      rm_f(INSTALL_RECORD)
    end
  private
    def paths
      @paths ||= (
        lines = File.read(INSTALL_RECORD).split(/\s*\n/)
        lines = lines.map{ |line| line.strip }
        lines = lines.uniq
        lines = lines.reject{ |line| line.empty? }       # skip blank lines
        lines = lines.reject{ |line| line[0,1] == '#' }  # skip blank lines
        lines
      )
    end
  end
end
require 'optparse'
module Setup
  class Command
    def self.run(*argv)
      new.run(*argv)
    end
    def self.tasks
      @tasks ||= {}
    end
    def self.order
      @order ||= []
    end
    def self.task(name, description)
      tasks[name] = description
      order << name
    end
    task 'show'     , "show current configuration"
    task 'all'      , "config, compile and install"
    task 'config'   , "save/customize configuration settings"
    task 'compile'  , "compile ruby extentions"
    task 'test'     , "run test suite"
    task 'install'  , "install project files"
    task 'clean'    , "does `make clean' for each extention"
    task 'distclean', "does `make distclean' for each extention"
    task 'uninstall', "uninstall previously installed files"
    def run(*argv)
      ARGV.replace(argv) unless argv.empty?
      task = ARGV.find{ |a| a !~ /^[-]/ }
      task = 'all' unless task
      unless task_names.include?(task)
        $stderr.puts "Not a valid task -- #{task}"
        exit 1
      end
      parser  = OptionParser.new
      options = {}
      parser.banner = "Usage: #{File.basename($0)} [TASK] [OPTIONS]"
      optparse_header(parser, options)
      case task
      when 'config'
        optparse_config(parser, options)
      when 'compile'
        optparse_compile(parser, options)
      when 'install'
        optparse_install(parser, options)
      when 'all'
        optparse_all(parser, options)
      end
      optparse_common(parser, options)
      begin
        parser.parse!(ARGV)
      rescue OptionParser::InvalidOption
        $stderr.puts $!.to_s.capitalize
        exit 1
      end
      rootdir = session.project.rootdir
      print_header
      begin
        $stderr.puts "(#{RUBY_ENGINE} #{RUBY_VERSION} #{RUBY_PLATFORM})"
      rescue
        $stderr.puts "(#{RUBY_VERSION} #{RUBY_PLATFORM})"
      end
      begin
        session.__send__(task)
      rescue Error => err
        raise err if $DEBUG
        $stderr.puts $!.message
        $stderr.puts "Try 'setup.rb --help' for detailed usage."
        abort $!.message #exit 1
      end
      puts unless session.quiet?
    end
    def session
      @session ||= Session.new(:io=>$stdout)
    end
    def configuration
      @configuration ||= session.configuration
    end
    def optparse_header(parser, options)
      parser.banner = "USAGE: #{File.basename($0)} [command] [options]"
    end
    def optparse_all(parser, options)
      optparse_config(parser, options)
      optparse_compile(parser, options)
      optparse_install(parser, options)  # TODO: why was this remarked out ?
    end
    def optparse_config(parser, options)
      parser.separator ""
      parser.separator "Configuration options:"
      configuration.options.each do |args|
        args = args.dup
        desc = args.pop
        type = args.pop
        name, shortcut = *args
        optname = name.to_s.gsub('_', '-')
        case type
        when :bool
          if optname.index('no-') == 0
            optname = "[no-]" + optname.sub(/^no-/, '')
            opts = shortcut ? ["-#{shortcut}", "--#{optname}", desc] : ["--#{optname}", desc]
            parser.on(*opts) do |val|
              configuration.__send__("#{name}=", !val)
            end
          else
            optname = "[no-]" + optname.sub(/^no-/, '')
            opts = shortcut ? ["-#{shortcut}", "--#{optname}", desc] : ["--#{optname}", desc]
            parser.on(*opts) do |val|
              configuration.__send__("#{name}=", val)
            end
          end
        else
          opts = shortcut ? ["-#{shortcut}", "--#{optname} #{type.to_s.upcase}", desc] :
                            ["--#{optname} #{type.to_s.upcase}", desc]
          parser.on(*opts) do |val|
            configuration.__send__("#{name}=", val)
          end
        end
      end
    end
    def optparse_compile(parser, options)
    end
    def optparse_install(parser, options)
      parser.separator ''
      parser.separator 'Install options:'
      parser.on('--prefix PATH', 'install to alternate root location') do |val|
        configuration.install_prefix = val
      end
      parser.on('--type TYPE', "install location mode (site,std,home)") do |val|
        configuration.type = val
      end
      parser.on('-t', '--[no-]test', "run pre-installation tests") do |bool|
        configuration.test = bool
      end
    end
    def optparse_common(parser, options)
      parser.separator ""
      parser.separator "General options:"
      parser.on("-q", "--quiet", "Suppress output") do
        session.quiet = true
      end
      parser.on("-f", "--force", "Force operation") do
        session.force = true
      end
      parser.on("--trace", "--verbose", "Watch execution") do |val|
        session.trace = true
      end
      parser.on("--trial", "--no-harm", "Do not write to disk") do |val|
        session.trial = true
      end
      parser.on("--debug", "Turn on debug mode") do |val|
        $DEBUG = true
      end
      parser.separator ""
      parser.separator "Inform options:"
      parser.on_tail("-h", "--help", "display this help information") do
        puts parser
        exit
      end
      parser.on_tail("--version", "-v", "Show version") do
        puts File.basename($0) + ' v' + Setup::VERSION #Version.join('.')
        exit
      end
      parser.on_tail("--copyright", "Show copyright") do
        puts Setup::COPYRIGHT #opyright
        exit
      end
    end
    def task_names
      self.class.tasks.keys
    end
    def print_header
    end
  end
end
Setup::Command.run