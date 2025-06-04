require_relative "test_helper"

class NArrayRactorTest < NArrayTestBase
  if respond_to?(:ractor)
    ractor keep: true
    data(:dtype, TYPES, keep: true)
    def test_non_frozen(data)
      dtype = data.fetch(:dtype)
      ary = random_array(dtype)
      r = Ractor.new(ary) {|x| x }
      # Use Ractor#value in Ruby 3.5+, fallback to #take for older versions
      ary2 = r.respond_to?(:value) ? r.value : r.take
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
      # Use Ractor#value in Ruby 3.5+, fallback to #take for older versions
      ary2, res = r.respond_to?(:value) ? r.value : r.take
      assert_equal((dtype != Numo::RObject),
                   ary1.equal?(ary2))
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
      # Use Ractor#value in Ruby 3.5+, fallback to #take for older versions
      result1 = r1.respond_to?(:value) ? r1.value : r1.take
      result2 = r2.respond_to?(:value) ? r2.value : r2.take
      assert_equal(result1, result2)
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
end
