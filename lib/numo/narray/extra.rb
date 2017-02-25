module Numo
  class NArray

    # p a = Numo::DFloat[[1, 2], [3, 4]]
    # # Numo::DFloat#shape=[2,2]
    # # [[1, 2],
    # #  [3, 4]]
    #
    # p b = Numo::DFloat[[5, 6]]
    # # Numo::DFloat#shape=[1,2]
    # # [[5, 6]]
    # # [[2], [2]]
    #
    # p a.concatenate(b,axis:0)
    # # Numo::DFloat#shape=[3,2]
    # # [[1, 2],
    # #  [3, 4],
    # #  [5, 6]]
    #
    # p a.concatenate(b.transpose, axis:1)
    # # Numo::DFloat#shape=[2,3]
    # # [[1, 2, 5],
    # #  [3, 4, 6]]

    def concatenate(*arrays,axis:0)
      check_axis(axis)
      self_shape = shape
      self_shape.delete_at(axis)
      sum_size = shape[axis]
      arrays.map! do |a|
        case a
        when NArray
          # ok
        when Numeric
          a = self.class.new(1)
          a.store(1)
        when Array
          a = self.class.cast(a)
        else
          raise TypeError,"not Numo::NArray"
        end
        if a.ndim != ndim
          raise ShapeError,"dimension mismatch"
        end
        a_shape = a.shape
        a_shape.delete_at(axis)
        if self_shape != a_shape
          raise ShapeError,"shape mismatch"
        end
        sum_size += a.shape[axis]
        a
      end
      self_shape.insert(axis,sum_size)
      result = self.class.zeros(*self_shape)
      lst = shape[axis]
      refs = [true] * ndim
      refs[axis] = 0...lst
      result[*refs] = self
      arrays.each do |a|
        fst = lst
        lst = fst + a.shape[axis]
        refs[axis] = fst...lst
        result[*refs] = a
      end
      result
    end

    # p x = Numo::DFloat.new(9).seq
    # # Numo::DFloat#shape=[9]
    # # [0, 1, 2, 3, 4, 5, 6, 7, 8]
    #
    # pp x.split(3)
    # # [Numo::DFloat(view)#shape=[3]
    # # [0, 1, 2],
    # #  Numo::DFloat(view)#shape=[3]
    # # [3, 4, 5],
    # #  Numo::DFloat(view)#shape=[3]
    # # [6, 7, 8]]
    #
    # p x = Numo::DFloat.new(8).seq
    # # Numo::DFloat#shape=[8]
    # # [0, 1, 2, 3, 4, 5, 6, 7]
    #
    # pp x.split([3, 5, 6, 10])
    # # [Numo::DFloat(view)#shape=[3]
    # # [0, 1, 2],
    # #  Numo::DFloat(view)#shape=[2]
    # # [3, 4],
    # #  Numo::DFloat(view)#shape=[1]
    # # [5],
    # #  Numo::DFloat(view)#shape=[2]
    # # [6, 7],
    # #  Numo::DFloat(view)#shape=[0][]]

    def split(indices_or_sections, axis:0)
      check_axis(axis)
      size_axis = shape[axis]
      case indices_or_sections
      when Integer
        div_axis, mod_axis = size_axis.divmod(indices_or_sections)
        if mod_axis != 0
          raise "not equally divide the axis"
        end
        refs = [true]*ndim
        indices_or_sections.times.map do |i|
          refs[axis] = i*div_axis ... (i+1)*div_axis
          self[*refs]
        end
      when NArray
        split(indices_or_sections.to_a,axis:axis)
      when Array
        refs = [true]*ndim
        fst = 0
        (indices_or_sections + [size_axis]).map do |lst|
          lst = size_axis if lst > size_axis
          refs[axis] = (fst < size_axis) ? fst...lst : -1...-1
          fst = lst
          self[*refs]
        end
      else
        raise TypeError,"argument must be Integer or Array"
      end
    end

    # p x = Numo::DFloat.new(4,4).seq
    # # Numo::DFloat#shape=[4,4]
    # # [[0, 1, 2, 3],
    # #  [4, 5, 6, 7],
    # #  [8, 9, 10, 11],
    # #  [12, 13, 14, 15]]
    #
    # pp x.hsplit(2)
    # # [Numo::DFloat(view)#shape=[4,2]
    # # [[0, 1],
    # #  [4, 5],
    # #  [8, 9],
    # #  [12, 13]],
    # #  Numo::DFloat(view)#shape=[4,2]
    # # [[2, 3],
    # #  [6, 7],
    # #  [10, 11],
    # #  [14, 15]]]
    #
    # pp x.hsplit([3, 6])
    # # [Numo::DFloat(view)#shape=[4,3]
    # # [[0, 1, 2],
    # #  [4, 5, 6],
    # #  [8, 9, 10],
    # #  [12, 13, 14]],
    # #  Numo::DFloat(view)#shape=[4,1]
    # # [[3],
    # #  [7],
    # #  [11],
    # #  [15]],
    # #  Numo::DFloat(view)#shape=[4,0][]]

    def vsplit(indices_or_sections)
      split(indices_or_sections, axis:0)
    end

    def hsplit(indices_or_sections)
      split(indices_or_sections, axis:1)
    end

    def dsplit(indices_or_sections)
      split(indices_or_sections, axis:2)
    end


    def check_axis(axis)
      if axis < 0
        axis += ndim
      end
      if axis < 0 || axis >= ndim
        raise ArgumentError,"invalid axis"
      end
    end

  end
end
