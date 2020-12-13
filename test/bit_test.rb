require_relative "test_helper"

class BitTest < Test::Unit::TestCase
  dtype = Numo::Bit

  test dtype do
    assert { dtype < Numo::NArray }
  end

  procs = [
    [proc{|tp,a| tp[*a] },""],
    [proc{|tp,a| tp[*a][true] },"[true]"],
    [proc{|tp,a| tp[*a][0..-1] },"[0..-1]"]
  ]
  procs.each do |init, ref|

    test "#{dtype},[0,1,1,0,1,0,0,1]#{ref}" do
      src = [0,1,1,0,1,0,0,1]
      n = src.size
      a = init.call(dtype, src)

      assert { a == src }
      assert { (a & 0) == [0] * n }
      assert { (a & 1) == src }
      assert { (a | 0) == src }
      assert { (a | 1) == [1] * n }
      assert { (a ^ 0) == src.map {|x| x ^ 0 } }
      assert { (a ^ 1) == src.map {|x| x ^ 1 } }
      assert { ~a == src.map {|x| 1 - x } }

      assert { a.count_true == 4 }
      assert { a.count_false == 4 }
      assert { a.where == [1,2,4,7] }
      assert { a.where2 == [[1,2,4,7], [0,3,5,6]] }
      assert { a.mask(Numo::DFloat[1,2,3,4,5,6,7,8]) == [2,3,5,8] }
      assert { !a.all? }
      assert { a.any? }
      assert { !a.none? }
    end
  end

  procs = [
    [proc{|tp,a| tp[*a] },""],
    [proc{|tp,a| tp[*a][true,0..-1] },"[true,true]"],
  ]
  procs.each do |init, ref|

    test "#{dtype},[[0,1,1,0],[1,0,0,1]]#{ref}" do
      src = [[0,1,1,0],[1,0,0,1]]
      n = src.size
      a = init.call(dtype, src)

      assert { a[5] == 0 }
      assert { a[-1] == 1 }
      assert { a[1,0] == src[1][0] }
      assert { a[1,1] == src[1][1] }
      assert { a[1,2] == src[1][2] }
      assert { a[3..4] == [0,1] }
      assert { a[0,1..2] == [1,1] }
      assert { a[0,:*] == src[0] }
      assert { a[1,:*] == src[1] }
      assert { a[:*,1] == [src[0][1], src[1][1]] }

      assert { a.count_true == 4 }
      assert { a.count_false == 4 }
      assert { a.where == [1,2,4,7] }
      assert { a.where2 == [[1,2,4,7],[0,3,5,6]] }
      assert { a.mask(Numo::DFloat[[1,2,3,4],[5,6,7,8]]) == [2,3,5,8] }
      assert { !a.all? }
      assert { a.any? }
      assert { !a.none? }
    end
  end

  procs = [
    [proc{|tp,a| tp[*a] },""],
  ]
  procs.each do |init, ref|

    test "#{dtype},[]#{ref}" do
      src = []
      n = src.size
      a = init.call(dtype, src)

      assert { a == src }
      assert { (a & 0) == [0] * n }
      assert { (a & 1) == src }
      assert { (a | 0) == src }
      assert { (a | 1) == [1] * n }
      assert { (a ^ 0) == src.map {|x| x ^ 0 } }
      assert { (a ^ 1) == src.map {|x| x ^ 1 } }
      assert { ~a == src.map {|x| 1 - x } }

      assert { a.count_true == 0 }
      assert { a.count_false == 0 }
      assert { a.where == [] }
      assert { a.where2 == [[], []] }
      assert { a.mask(Numo::DFloat[]) == [] }
      assert { !a.all? }
      assert { !a.any? }
      assert { a.none? }
    end

  end

  test "store to view" do
    n=14
    x = Numo::Bit.zeros(n+2,n+2,3)
    ~(x[1..-2, 1..-2, 0].inplace)
    assert { x.where.size == n*n}

    x1 = Numo::Bit.ones(n,n)
    x0 = Numo::Bit.zeros(n,n)
    y0 = Numo::Bit.zeros(n+2,n+2)
    x = Numo::NArray.dstack([x1, x0, x0])
    y = Numo::NArray.dstack([y0, y0, y0])
    y[1..-2, 1..-2, true] = x
    assert { (~y[1..-2, 1..-2, 0]).where.size == 0}
    assert { y[true,true,1].where.size == 0 }
  end

  test "assign nil" do
    x = Numo::RObject.cast([1, 2, 3])
    x[Numo::Bit.cast([0, 1, 0])] = nil
    assert { x.to_a == [1, nil, 3] }
  end

  test "flipped matrices > 6x6" do
    m = Numo::Int32.zeros(7,7)
    m = m.flipud
    m[true, 1] = 1
    assert{ m == [[0, 1, 0, 0, 0, 0, 0],
                  [0, 1, 0, 0, 0, 0, 0],
                  [0, 1, 0, 0, 0, 0, 0],
                  [0, 1, 0, 0, 0, 0, 0],
                  [0, 1, 0, 0, 0, 0, 0],
                  [0, 1, 0, 0, 0, 0, 0],
                  [0, 1, 0, 0, 0, 0, 0]] }
  end
end
