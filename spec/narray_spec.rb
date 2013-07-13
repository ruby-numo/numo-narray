require File.join(File.dirname(__FILE__), "../ext/narray")
#NArray.debug = true

RSpec.configure do |config|
  config.treat_symbols_as_metadata_keys_with_true_values = true
  config.filter_run :focus
  config.run_all_when_everything_filtered = true
end
#context :focus=>true do ... end

types = [
  NArray::DFloat,
  NArray::SFloat,
  NArray::DComplex,
  NArray::SComplex,
  NArray::Int64,
  NArray::Int32,
  NArray::Int16,
  NArray::Int8,
  NArray::UInt64,
  NArray::UInt32,
  NArray::UInt16,
  NArray::UInt8,
]
types.each do |dtype|

  #describe dtype do
  #  it{should be_kind_of Class}
  #end

  procs = [
    [proc{|tp| tp.cast([1,2,3,5,7,11]) },""],
    [proc{|tp| tp.cast([1,2,3,5,7,11])[true] },"[true]"],
    [proc{|tp| tp.cast([1,2,3,5,7,11])[0..-1] },"[0..-1]"]
  ]
  procs.each do |init,ref|

    describe init.call(dtype) do
      it{should be_kind_of dtype}
      its(:size){should == 6}
      its(:ndim){should == 1}
      its(:shape){should == [6]}

      it{should_not be_inplace}
      it{should     be_row_major}
      it{should_not be_column_major}
      it{should     be_host_order}
      it{should_not be_byte_swapped}

      its(:to_a){should == [1,2,3,5,7,11]}
      its(:to_a){should be_kind_of Array}
      it{should == [1,2,3,5,7,11]}
    end

    #it "should be inspect ..." do
    #  s = "#{dtype.to_s}( #shape=[6]\n[1, 2, 3, 5, 7, 11]\n)"
    #  @a.inspect.should == s
    #end

    describe dtype,".cast([1,2,3,5,7,11])"+ref do
      before do
        @a = init.call(dtype)
      end

      it "eq [1,1,3,3,7,7]" do
        @a.eq([1,1,3,3,7,7]).should == [1,0,1,0,1,0]
      end

      it "[3..4]" do
        @a[3..4].should == [5,7]
      end

      it "['1::2']" do
        @a['1::2'].should == [2,5,11]
      end

      it "[5]" do
        @a[5].should == 11
      end

      it "[-1]" do
        @a[-1].should == 11
      end

      it ".sum" do
        @a.sum.should == 29
      end

      it ".fill(12)" do
        @a.fill(12).should == [12]*6
      end

      it "+ 1" do
        (@a + 1).should == [2,3,4,6,8,12]
      end

      it "- 1" do
        (@a - 1).should == [0,1,2,4,6,10]
      end

      it "* 3" do
        (@a * 3).should == [3,6,9,15,21,33]
      end

      it "/ 0.5" do
        (@a / 0.5).should == [2,4,6,10,14,22]
      end

      it "unary minus" do
        (-@a).should == [-1,-2,-3,-5,-7,-11]
      end

      it "** 2" do
        (@a ** 2).should == [1,4,9,25,49,121]
      end

      after do
        @a = nil
      end
    end
  end

  #describe dtype, ".seq(5)" do
  #  it do
  #    dtype.seq(5).should == [0,1,2,3,4]
  #  end
  #end
end
