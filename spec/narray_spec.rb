require "numo/narray"
#Numo::NArray.debug = true

RSpec.configure do |config|
  config.filter_run :focus
  config.run_all_when_everything_filtered = true
end
#context :focus=>true do ... end

types = [
  Numo::DFloat,
  Numo::SFloat,
  Numo::DComplex,
  Numo::SComplex,
  Numo::Int64,
  Numo::Int32,
  Numo::Int16,
  Numo::Int8,
  Numo::UInt64,
  Numo::UInt32,
  Numo::UInt16,
  Numo::UInt8,
]
#types = [Numo::DFloat]
float_types = [
  Numo::DFloat,
  Numo::DComplex,
]

types.each do |dtype|

  describe dtype  do
    it{expect(dtype).to be < Numo::NArray}
  end

  procs = [
    [proc{|tp,a| tp[*a] },""],
    [proc{|tp,a| tp[*a][true] },"[true]"],
    [proc{|tp,a| tp[*a][0..-1] },"[0..-1]"]
  ]
  procs.each do |init,ref|

    describe dtype,"[1,2,3,5,7,11]"+ref do
      before(:all) do
        @src = [1,2,3,5,7,11]
        @a = init.call(dtype,@src)
      end
      #context :focus=>true do

      it{expect(@a).to be_kind_of dtype}
      it{expect(@a.size).to eq 6}
      it{expect(@a.ndim).to eq 1}
      it{expect(@a.shape).to eq [6]}
      it{expect(@a).not_to be_inplace}
      it{expect(@a).to     be_row_major}
      it{expect(@a).not_to be_column_major}
      it{expect(@a).to     be_host_order}
      it{expect(@a).not_to be_byte_swapped}
      it{expect(@a).to eq [1,2,3,5,7,11]}
      it{expect(@a.to_a).to eq [1,2,3,5,7,11]}
      it{expect(@a.to_a).to be_kind_of Array}
      it{expect(@a.dup).to eq @a}
      it{expect(@a.clone).to eq @a}
      it{expect(@a.dup.object_id).not_to eq @a.object_id}
      it{expect(@a.clone.object_id).not_to eq @a.object_id}

      it{expect(@a.eq([1,1,3,3,7,7])).to eq [1,0,1,0,1,0]}
      it{expect(@a[3..4]).to eq [5,7]}
      it{expect(@a[5]).to eq 11}
      it{expect(@a[-1]).to eq 11}
      it{expect(@a[[4,3,0,1,5,2]]).to eq [7,5,1,2,11,3]}
      it{expect(@a.sum).to eq 29}
      if float_types.include?(dtype)
        it{expect(@a.mean).to eq 29.0/6}
        it{expect(@a.var).to eq 13.766666666666669}
        it{expect(@a.stddev).to eq 3.710345895825168}
        it{expect(@a.rms).to eq 5.901977069875258}
      end
      it{expect(@a.copy.fill(12)).to eq [12]*6}
      it{expect((@a + 1)).to eq [2,3,4,6,8,12]}
      it{expect((@a - 1)).to eq [0,1,2,4,6,10]}
      it{expect((@a * 3)).to eq [3,6,9,15,21,33]}
      it{expect((@a / 0.5)).to eq [2,4,6,10,14,22]}
      it{expect((-@a)).to eq [-1,-2,-3,-5,-7,-11]}
      it{expect((@a ** 2)).to eq [1,4,9,25,49,121]}
      it{expect(@a.swap_byte.swap_byte).to eq [1,2,3,5,7,11]}
      if dtype == Numo::DComplex || dtype == Numo::SComplex
        it{expect(@a.real).to eq @src}
        it{expect(@a.imag).to eq [0]*6}
        it{expect(@a.conj).to eq @src}
        it{expect(@a.angle).to eq [0]*6}
      else
        it{expect(@a.min).to eq 1}
        it{expect(@a.max).to eq 11}
        it{expect((@a >= 3)).to eq [0,0,1,1,1,1]}
        it{expect((@a >  3)).to eq [0,0,0,1,1,1]}
        it{expect((@a <= 3)).to eq [1,1,1,0,0,0]}
        it{expect((@a <  3)).to eq [1,1,0,0,0,0]}
        it{expect((@a.eq 3)).to eq [0,0,1,0,0,0]}
        it{expect(@a.sort).to eq @src}
        it{expect(@a.sort_index).to eq (0..5).to_a}
        it{expect(@a.median).to eq 4}
      end
    end
  end

  describe dtype, '[1..4]' do
    it{expect(dtype[1..4]).to eq [1,2,3,4]}
  end

  #describe dtype, ".seq(5)" do
  #  it do
  #    dtype.seq(5).should == [0,1,2,3,4]
  #  end
  #end

  procs2 = [
    [proc{|tp,src| tp[*src] },""],
    [proc{|tp,src| tp[*src][true,true] },"[true,true]"],
    [proc{|tp,src| tp[*src][0..-1,0..-1] },"[0..-1,0..-1]"]
  ]
  procs2.each do |init,ref|

    describe dtype,'[[1,2,3],[5,7,11]]'+ref do
      before(:all) do
        @src = [[1,2,3],[5,7,11]]
        @a = init.call(dtype,@src)
      end
      #context :focus=>true do

      it{expect(@a).to be_kind_of dtype}
      it{expect(@a.size).to eq 6}
      it{expect(@a.ndim).to eq 2}
      it{expect(@a.shape).to eq [2,3]}
      it{expect(@a).not_to be_inplace}
      it{expect(@a).to     be_row_major}
      it{expect(@a).not_to be_column_major}
      it{expect(@a).to     be_host_order}
      it{expect(@a).not_to be_byte_swapped}
      it{expect(@a).to eq @src}
      it{expect(@a.to_a).to eq @src}
      it{expect(@a.to_a).to be_kind_of Array}

      it{expect(@a.eq([[1,1,3],[3,7,7]])).to eq [[1,0,1],[0,1,0]]}
      it{expect(@a[5]).to eq 11}
      it{expect(@a[-1]).to eq 11}
      it{expect(@a[1,0]).to eq @src[1][0]}
      it{expect(@a[1,1]).to eq @src[1][1]}
      it{expect(@a[1,2]).to eq @src[1][2]}
      it{expect(@a[3..4]).to eq [5,7]}
      it{expect(@a[0,1..2]).to eq [2,3]}
      it{expect(@a[0,:*]).to eq @src[0]}
      it{expect(@a[1,:*]).to eq @src[1]}
      it{expect(@a[:*,1]).to eq [@src[0][1],@src[1][1]]}
      it{expect(@a[true,[2,0,1]]).to eq [[3,1,2],[11,5,7]]}
      it{expect(@a.reshape(3,2)).to eq [[1,2],[3,5],[7,11]]}
      it{expect(@a.reshape(3,nil)).to eq [[1,2],[3,5],[7,11]]}
      it{expect(@a.reshape(nil,2)).to eq [[1,2],[3,5],[7,11]]}
      it{expect(@a.transpose).to eq [[1,5],[2,7],[3,11]]}
      it{expect(@a.transpose(1,0)).to eq [[1,5],[2,7],[3,11]]}

      it{expect(@a.sum).to eq 29}
      it{expect(@a.sum(0)).to eq [6, 9, 14]}
      it{expect(@a.sum(1)).to eq [6, 23]}
      if float_types.include?(dtype)
        it{expect(@a.mean).to eq 29.0/6}
        it{expect(@a.mean(0)).to eq [3, 4.5, 7]}
        it{expect(@a.mean(1)).to eq [2, 23.0/3]}
      end
      if dtype == Numo::DComplex || dtype == Numo::SComplex
        it{expect(@a.real).to eq @src}
        it{expect(@a.imag).to eq [[0]*3]*2}
        it{expect(@a.conj).to eq @src}
        it{expect(@a.angle).to eq [[0]*3]*2}
      else
        it{expect(@a.min).to eq 1}
        it{expect(@a.max).to eq 11}
        it{expect((@a >= 3)).to eq [[0,0,1],[1,1,1]]}
        it{expect((@a >  3)).to eq [[0,0,0],[1,1,1]]}
        it{expect((@a <= 3)).to eq [[1,1,1],[0,0,0]]}
        it{expect((@a <  3)).to eq [[1,1,0],[0,0,0]]}
        it{expect((@a.eq 3)).to eq [[0,0,1],[0,0,0]]}
        it{expect(@a.sort).to eq @src}
        it{expect(@a.sort_index).to eq [[0,1,2],[3,4,5]]}
      end
      it{expect(@a.copy.fill(12)).to eq [[12]*3]*2}
      it{expect((@a + 1)).to eq [[2,3,4],[6,8,12]]}
      it{expect((@a + [1,2,3])).to eq [[2,4,6],[6,9,14]]}
      it{expect((@a - 1)).to eq [[0,1,2],[4,6,10]]}
      it{expect((@a - [1,2,3])).to eq [[0,0,0],[4,5,8]]}
      it{expect((@a * 3)).to eq [[3,6,9],[15,21,33]]}
      it{expect((@a * [1,2,3])).to eq [[1,4,9],[5,14,33]]}
      it{expect((@a / 0.5)).to eq [[2,4,6],[10,14,22]]}
      it{expect((-@a)).to eq [[-1,-2,-3],[-5,-7,-11]]}
      it{expect((@a ** 2)).to eq [[1,4,9],[25,49,121]]}
      it{expect((dtype[[1,0],[0,1]].dot dtype[[4,1],[2,2]])).to eq [[4,1],[2,2]]}
      it{expect(@a.swap_byte.swap_byte).to eq @src}
    end

  end

  describe dtype,"[[[1,2],[3,4]],[[5,6],[7,8]]]" do
    before do
      @a = dtype[[[1,2],[3,4]],[[5,6],[7,8]]]
    end

    it{expect(@a[0, 1, 1]).to eq 4}
    #it{expect(@a[:rest]).to eq @a} # note: this spec probably shows the correct behaviour
    it{expect(@a[0, :rest]).to eq [[1,2],[3,4]]}
    it{expect(@a[0, false]).to eq [[1,2],[3,4]]}
    it{expect(@a[0, 1, :rest]).to eq [3,4]}
    it{expect(@a[0, 1, false]).to eq [3,4]}
    it{expect(@a[:rest, 0]).to eq [[1,3],[5,7]]}
    it{expect(@a[:rest, 0, 1]).to eq [2,6]}
    it{expect(@a[1, :rest, 0]).to eq [5,7]}
    it{expect{@a[1, 1, :rest, 0]}.to raise_error IndexError}
    it{expect{@a[1, 1, 1, 1, :rest]}.to raise_error IndexError}
    it{expect{@a[1, 1, 1, :rest, 1]}.to raise_error IndexError}
  end

  describe dtype, "#dot" do
    it "vector.dot(vector)" do
      a = dtype[1..3]
      b = dtype[2..4]
      expect(a.dot(b)).to eq (1*2 + 2*3 + 3*4)
    end
    it "matrix.dot(vector)" do
      a = dtype[1..6].reshape(3,2)
      b = dtype[1..2]
      expect(a.dot(b)).to eq [5, 11, 17]
    end
    it "vector.dot(matrix)" do
      a = dtype[1..2]
      b = dtype[1..6].reshape(2,3)
      expect(a.dot(b)).to eq [9, 12, 15]
    end
    it "matrix.dot(matrix)" do
      a = dtype[1..6].reshape(3,2)
      b = dtype[1..6].reshape(2,3)
      expect(a.dot(b)).to eq [[9, 12, 15], [19, 26, 33], [29, 40, 51]]
      expect(b.dot(a)).to eq [[22, 28], [49, 64]]
    end
    it "matrix.dot(matrix) with incorrect shape" do
      a = dtype[1..6].reshape(3,2)
      b = dtype[1..9].reshape(3,3)
      expect{a.dot(b)}.to raise_error(TypeError)
    end
  end
end
