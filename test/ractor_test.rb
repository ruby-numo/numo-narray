require_relative "test_helper"

class NArrayRactorTest < NArrayTestBase
  def setup
    super
    skip unless defined? Ractor
  end

  data(:dtype, TYPES, keep: true)
  def test_non_frozen(data)
    dtype = data.fetch(:dtype)
    ary = random_array(dtype)
    r = Ractor.new(ary) {|x| x }
    ary2 = r.take
    assert_equal(ary, ary2)
    assert_not_same(ary, ary2)
  end

  def test_frozen(data)
    dtype = data.fetch(:dtype)
    ary1 = random_array(dtype)
    ary1.freeze
    r = Ractor.new(ary1) do |ary2|
      [ary2, ary2 * 10]
    end
    ary2, res = r.take
    assert_same(ary1, ary2)
    assert_equal(ary1*10, res)
  end

  def test_parallel(data)
    dtype = data.fetch(:dtype)
    ary1 = random_array(dtype, 100000)
    r1 = Ractor.new(ary1) do |ary2|
      ary2 * 10
    end
    r2 = Ractor.new(ary1) do |ary4|
      ary4 * 10
    end
    assert_equal(r1.take, r2.take)
  end

  def random_array(dtype, n=1000)
    case dtype
    when Numo::DFloat, Numo::SFloat, Numo::DComplex, Numo::SComplex
      dtype.new(n).rand_norm
    else
      dtype.new(n).rand(10)
    end
  end
end
