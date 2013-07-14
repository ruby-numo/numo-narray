require File.join(File.dirname(__FILE__), "../ext/narray")
#NArray.debug = true

RSpec.configure do |config|
  config.treat_symbols_as_metadata_keys_with_true_values = true
  config.filter_run :focus
  config.run_all_when_everything_filtered = true
end
#context :focus=>true do ... end

dtype = NArray::Bit

describe dtype do
  it{expect(dtype).to be < NArray}
end

procs = [
  [proc{|tp,a| tp[*a] },""],
  [proc{|tp,a| tp[*a][true] },"[true]"],
  [proc{|tp,a| tp[*a][0..-1] },"[0..-1]"]
]
procs.each do |init,ref|

  describe dtype,"[0,1,1,0,1,0,0,1]"+ref do
    before(:all) do
      @src = [0,1,1,0,1,0,0,1]
      @n = @src.size
      @a = init.call(dtype,@src)
    end

    it{expect(@a).to eq @src}
    it{expect(@a & 0).to eq [0]*@n}
    it{expect(@a & 1).to eq @src}
    it{expect(@a | 0).to eq @src}
    it{expect(@a | 1).to eq [1]*@n}
    it{expect(@a ^ 0).to eq @src.map{|x| x^0}}
    it{expect(@a ^ 1).to eq @src.map{|x| x^1}}
    #it{expect(~@a).to be eq @src.map{|x| ~x}}

    after(:all) do
      @a = nil
    end
  end

end

procs = [
  [proc{|tp,a| tp[*a] },""],
  [proc{|tp,a| tp[*a][true,0..-1] },"[true,true]"],
]
procs.each do |init,ref|

  describe dtype,"[[0,1,1,0],[1,0,0,1]]"+ref do
    before(:all) do
      @src = [[0,1,1,0],[1,0,0,1]]
      @n = @src.size
      @a = init.call(dtype,@src)
    end

    it{expect(@a[5]).to eq 0}
    it{expect(@a[-1]).to eq 1}
    it{expect(@a[1,0]).to eq @src[1][0]}
    it{expect(@a[1,1]).to eq @src[1][1]}
    it{expect(@a[1,2]).to eq @src[1][2]}
    it{expect(@a['1::2']).to eq [1,0,0,1]}
    it{expect(@a[3..4]).to eq [0,1]}
    it{expect(@a[0,1..2]).to eq [1,1]}
    it{expect(@a['1,0::2']).to eq [1,0]}
    it{expect(@a[0,:*]).to eq @src[0]}
    it{expect(@a[1,:*]).to eq @src[1]}
    it{expect(@a[:*,1]).to eq [@src[0][1],@src[1][1]]}

    after(:all) do
      @a = nil
    end
  end

end
