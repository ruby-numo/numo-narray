require File.join(File.dirname(__FILE__), "../ext/narray")

types = [
  NArray::DFloat,
  NArray::SFloat,
  NArray::DComplex,
  NArray::SComplex,
  NArray::Int64,
  NArray::Int32,
  NArray::Int16,
  NArray::Int8,
]

types.each do |dtype|
  describe dtype, ".cast([1,2,3,5,7,11])" do
    before do
      @a = dtype.cast([1,2,3,5,7,11])
    end

    it "should be kind_of #{dtype.to_s}" do
      @a.should be_kind_of dtype
    end

    it "should be ndim 1" do
      @a.ndim.should == 1
    end

    it "should be shape [6]" do
      @a.shape.should == [6]
    end

    it "should be size 6" do
      @a.size.should == 6
    end

    #it "should be inspect ..." do
    #  s = "#{dtype.to_s}( #shape=[6]\n[1, 2, 3, 5, 7, 11]\n)"
    #  @a.inspect.should == s
    #end

    it ".to_a.kind_of? Array" do
      @a.to_a.should be_kind_of Array
    end

    it "== [1,2,3,5,7,11]" do
      @a.should == [1,2,3,5,7,11]
    end

    it "[3..4]" do
      @a[3..4].should == [5,7]
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

    after do
      @a = nil
    end
  end
end
