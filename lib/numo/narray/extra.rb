module Numo
  class NArray

    # Return an unallocated array with the same shape and type as self.
    def new_narray
      self.class.new(*shape)
    end

    # Return an array of zeros with the same shape and type as self.
    def new_zeros
      self.class.zeros(*shape)
    end

    # Return an array of ones with the same shape and type as self.
    def new_ones
      self.class.ones(*shape)
    end

    # Return an array filled with value with the same shape and type as self.
    def new_fill(value)
      self.class.new(*shape).fill(value)
    end

    # Convert angles from radians to degrees.
    def rad2deg
      self * (180/Math::PI)
    end

    # Convert angles from degrees to radians.
    def deg2rad
      self * (Math::PI/180)
    end

    # Flip each row in the left/right direction.
    # Same as a[true, (-1..0).step(-1), ...].
    def fliplr
      reverse(1)
    end

    # Flip each column in the up/down direction.
    # Same as a[(-1..0).step(-1), ...].
    def flipud
      reverse(0)
    end

    # Multi-dimensional array indexing.
    # Same as [] for one-dimensional NArray.
    # Similar to numpy's tuple indexing, i.e., a[[1,2,..],[3,4,..]]
    # (This method will be rewritten in C)
    # @return [Numo::NArray] one-dimensional view of self.
    # @example
    #   p x = Numo::DFloat.new(3,3,3).seq
    #   # Numo::DFloat#shape=[3,3,3]
    #   # [[[0, 1, 2],
    #   #   [3, 4, 5],
    #   #   [6, 7, 8]],
    #   #  [[9, 10, 11],
    #   #   [12, 13, 14],
    #   #   [15, 16, 17]],
    #   #  [[18, 19, 20],
    #   #   [21, 22, 23],
    #   #   [24, 25, 26]]]
    #
    #   p x.at([0,1,2],[0,1,2],[-1,-2,-3])
    #   # Numo::DFloat(view)#shape=[3]
    #   # [2, 13, 24]
    def at(*indices)
      if indices.size != ndim
        raise DimensionError, "argument length does not match dimension size"
      end
      idx = nil
      stride = 1
      (indices.size-1).downto(0) do |i|
        ix = Int64.cast(indices[i])
        if ix.ndim != 1
          raise DimensionError, "index array is not one-dimensional"
        end
        ix[ix < 0] += shape[i]
        if ((ix < 0) & (ix >= shape[i])).any?
          raise IndexError, "index array is out of range"
        end
        if idx
          if idx.size != ix.size
            raise ShapeError, "index array sizes mismatch"
          end
          idx += ix * stride
          stride *= shape[i]
        else
          idx = ix
          stride = shape[i]
        end
      end
      self[idx]
    end

    # Rotate in the plane specified by axes.
    # @example
    #   p a = Numo::Int32.new(2,2).seq
    #   # Numo::Int32#shape=[2,2]
    #   # [[0, 1],
    #   #  [2, 3]]
    #
    #   p a.rot90
    #   # Numo::Int32(view)#shape=[2,2]
    #   # [[1, 3],
    #   #  [0, 2]]
    #
    #   p a.rot90(2)
    #   # Numo::Int32(view)#shape=[2,2]
    #   # [[3, 2],
    #   #  [1, 0]]
    #
    #   p a.rot90(3)
    #   # Numo::Int32(view)#shape=[2,2]
    #   # [[2, 0],
    #   #  [3, 1]]
    def rot90(k=1,axes=[0,1])
      case k % 4
      when 0
        view
      when 1
        swapaxes(*axes).reverse(axes[0])
      when 2
        reverse(*axes)
      when 3
        swapaxes(*axes).reverse(axes[1])
      end
    end

    def to_i
      if size==1
        self[0].to_i
      else
        # convert to Int?
        raise TypeError, "can't convert #{self.class} into Integer"
      end
    end

    def to_f
      if size==1
        self[0].to_f
      else
        # convert to DFloat?
        raise TypeError, "can't convert #{self.class} into Float"
      end
    end

    def to_c
      if size==1
        Complex(self[0])
      else
        # convert to DComplex?
        raise TypeError, "can't convert #{self.class} into Complex"
      end
    end

    # Convert the argument to an narray if not an narray.
    def self.cast(a)
      a.kind_of?(NArray) ? a : NArray.array_type(a).cast(a)
    end

    def self.asarray(a)
      case a
      when NArray
        a
      when Numeric,Range
        self[a]
      else
        cast(a)
      end
    end

    # Append values to the end of an narray.
    # @example
    #   a = Numo::DFloat[1, 2, 3]
    #   p a.append([[4, 5, 6], [7, 8, 9]])
    #   # Numo::DFloat#shape=[9]
    #   # [1, 2, 3, 4, 5, 6, 7, 8, 9]
    #
    #   a = Numo::DFloat[[1, 2, 3]]
    #   p a.append([[4, 5, 6], [7, 8, 9]],axis:0)
    #   # Numo::DFloat#shape=[3,3]
    #   # [[1, 2, 3],
    #   #  [4, 5, 6],
    #   #  [7, 8, 9]]
    #
    #   a = Numo::DFloat[[1, 2, 3], [4, 5, 6]]
    #   p a.append([7, 8, 9], axis:0)
    #   # in `append': dimension mismatch (Numo::NArray::DimensionError)

    def append(other,axis:nil)
      other = self.class.cast(other)
      if axis
        if ndim != other.ndim
          raise DimensionError, "dimension mismatch"
        end
        return concatenate(other,axis:axis)
      else
        a = self.class.zeros(size+other.size)
        a[0...size] = self[true]
        a[size..-1] = other[true]
        return a
      end
    end

    # Return a new array with sub-arrays along an axis deleted.
    # If axis is not given, obj is applied to the flattened array.

    # @example
    #   a = Numo::DFloat[[1,2,3,4], [5,6,7,8], [9,10,11,12]]
    #   p a.delete(1,0)
    #   # Numo::DFloat(view)#shape=[2,4]
    #   # [[1, 2, 3, 4],
    #   #  [9, 10, 11, 12]]
    #
    #   p a.delete((0..-1).step(2),1)
    #   # Numo::DFloat(view)#shape=[3,2]
    #   # [[2, 4],
    #   #  [6, 8],
    #   #  [10, 12]]
    #
    #   p a.delete([1,3,5])
    #   # Numo::DFloat(view)#shape=[9]
    #   # [1, 3, 5, 7, 8, 9, 10, 11, 12]

    def delete(indice,axis=nil)
      if axis
        bit = Bit.ones(shape[axis])
        bit[indice] = 0
        idx = [true]*ndim
        idx[axis] = bit.where
        return self[*idx].copy
      else
        bit = Bit.ones(size)
        bit[indice] = 0
        return self[bit.where].copy
      end
    end

    # Insert values along the axis before the indices.
    # @example
    #   p a = Numo::DFloat[[1, 2], [3, 4]]
    #   a = Numo::Int32[[1, 1], [2, 2], [3, 3]]
    #
    #   p a.insert(1,5)
    #   # Numo::Int32#shape=[7]
    #   # [1, 5, 1, 2, 2, 3, 3]
    #
    #   p a.insert(1, 5, axis:1)
    #   # Numo::Int32#shape=[3,3]
    #   # [[1, 5, 1],
    #   #  [2, 5, 2],
    #   #  [3, 5, 3]]
    #
    #   p a.insert([1], [[11],[12],[13]], axis:1)
    #   # Numo::Int32#shape=[3,3]
    #   # [[1, 11, 1],
    #   #  [2, 12, 2],
    #   #  [3, 13, 3]]
    #
    #   p a.insert(1, [11, 12, 13], axis:1)
    #   # Numo::Int32#shape=[3,3]
    #   # [[1, 11, 1],
    #   #  [2, 12, 2],
    #   #  [3, 13, 3]]
    #
    #   p a.insert([1], [11, 12, 13], axis:1)
    #   # Numo::Int32#shape=[3,5]
    #   # [[1, 11, 12, 13, 1],
    #   #  [2, 11, 12, 13, 2],
    #   #  [3, 11, 12, 13, 3]]
    #
    #   p b = a.flatten
    #   # Numo::Int32(view)#shape=[6]
    #   # [1, 1, 2, 2, 3, 3]
    #
    #   p b.insert(2,[15,16])
    #   # Numo::Int32#shape=[8]
    #   # [1, 1, 15, 16, 2, 2, 3, 3]
    #
    #   p b.insert([2,2],[15,16])
    #   # Numo::Int32#shape=[8]
    #   # [1, 1, 15, 16, 2, 2, 3, 3]
    #
    #   p b.insert([2,1],[15,16])
    #   # Numo::Int32#shape=[8]
    #   # [1, 16, 1, 15, 2, 2, 3, 3]
    #
    #   p b.insert([2,0,1],[15,16,17])
    #   # Numo::Int32#shape=[9]
    #   # [16, 1, 17, 1, 15, 2, 2, 3, 3]
    #
    #   p b.insert(2..3, [15, 16])
    #   # Numo::Int32#shape=[8]
    #   # [1, 1, 15, 2, 16, 2, 3, 3]
    #
    #   p b.insert(2, [7.13, 0.5])
    #   # Numo::Int32#shape=[8]
    #   # [1, 1, 7, 0, 2, 2, 3, 3]
    #
    #   p x = Numo::DFloat.new(2,4).seq
    #   # Numo::DFloat#shape=[2,4]
    #   # [[0, 1, 2, 3],
    #   #  [4, 5, 6, 7]]
    #
    #   p x.insert([1,3],999,axis:1)
    #   # Numo::DFloat#shape=[2,6]
    #   # [[0, 999, 1, 2, 999, 3],
    #   #  [4, 999, 5, 6, 999, 7]]

    def insert(indice,values,axis:nil)
      if axis
        values = self.class.asarray(values)
        nd = values.ndim
        midx = [:new]*(ndim-nd) + [true]*nd
        case indice
        when Numeric
          midx[-nd-1] = true
          midx[axis] = :new
        end
        values = values[*midx]
      else
        values = self.class.asarray(values).flatten
      end
      idx = Int64.asarray(indice)
      nidx = idx.size
      if nidx == 1
        nidx = values.shape[axis||0]
        idx = idx + Int64.new(nidx).seq
      else
        sidx = idx.sort_index
        idx[sidx] += Int64.new(nidx).seq
      end
      if axis
        bit = Bit.ones(shape[axis]+nidx)
        bit[idx] = 0
        new_shape = shape
        new_shape[axis] += nidx
        a = self.class.zeros(new_shape)
        mdidx = [true]*ndim
        mdidx[axis] = bit.where
        a[*mdidx] = self
        mdidx[axis] = idx
        a[*mdidx] = values
      else
        bit = Bit.ones(size+nidx)
        bit[idx] = 0
        a = self.class.zeros(size+nidx)
        a[bit.where] = self.flatten
        a[idx] = values
      end
      return a
    end

    class << self
    # @example
    #   p a = Numo::DFloat[[1, 2], [3, 4]]
    #   # Numo::DFloat#shape=[2,2]
    #   # [[1, 2],
    #   #  [3, 4]]
    #
    #   p b = Numo::DFloat[[5, 6]]
    #   # Numo::DFloat#shape=[1,2]
    #   # [[5, 6]]
    #
    #   p Numo::NArray.concatenate([a,b],axis:0)
    #   # Numo::DFloat#shape=[3,2]
    #   # [[1, 2],
    #   #  [3, 4],
    #   #  [5, 6]]
    #
    #   p Numo::NArray.concatenate([a,b.transpose], axis:1)
    #   # Numo::DFloat#shape=[2,3]
    #   # [[1, 2, 5],
    #   #  [3, 4, 6]]

    def concatenate(arrays,axis:0)
      klass = (self==NArray) ? NArray.array_type(arrays) : self
      nd = 0
      arrays = arrays.map do |a|
        case a
        when NArray
          # ok
        when Numeric
          a = klass[a]
        when Array
          a = klass.cast(a)
        else
          raise TypeError,"not Numo::NArray: #{a.inspect[0..48]}"
        end
        if a.ndim > nd
          nd = a.ndim
        end
        a
      end
      if axis < 0
        axis += nd
      end
      if axis < 0 || axis >= nd
        raise ArgumentError,"axis is out of range"
      end
      new_shape = nil
      sum_size = 0
      arrays.each do |a|
        a_shape = a.shape
        if nd != a_shape.size
          a_shape = [1]*(nd-a_shape.size) + a_shape
        end
        sum_size += a_shape.delete_at(axis)
        if new_shape
          if new_shape != a_shape
            raise ShapeError,"shape mismatch"
          end
        else
          new_shape = a_shape
        end
      end
      new_shape.insert(axis,sum_size)
      result = klass.zeros(*new_shape)
      lst = 0
      refs = [true] * nd
      arrays.each do |a|
        fst = lst
        lst = fst + (a.shape[axis-nd]||1)
        refs[axis] = fst...lst
        result[*refs] = a
      end
      result
    end

    # Stack arrays vertically (row wise).
    # @example
    #   a = Numo::Int32[1,2,3]
    #   b = Numo::Int32[2,3,4]
    #   p Numo::NArray.vstack([a,b])
    #   # Numo::Int32#shape=[2,3]
    #   # [[1, 2, 3],
    #   #  [2, 3, 4]]
    #
    #   a = Numo::Int32[[1],[2],[3]]
    #   b = Numo::Int32[[2],[3],[4]]
    #   p Numo::NArray.vstack([a,b])
    #   # Numo::Int32#shape=[6,1]
    #   # [[1],
    #   #  [2],
    #   #  [3],
    #   #  [2],
    #   #  [3],
    #   #  [4]]

    def vstack(arrays)
      arys = arrays.map do |a|
        _atleast_2d(cast(a))
      end
      concatenate(arys,axis:0)
    end

    # Stack arrays horizontally (column wise).
    # @example
    #   a = Numo::Int32[1,2,3]
    #   b = Numo::Int32[2,3,4]
    #   p Numo::NArray.hstack([a,b])
    #   # Numo::Int32#shape=[6]
    #   # [1, 2, 3, 2, 3, 4]
    #
    #   a = Numo::Int32[[1],[2],[3]]
    #   b = Numo::Int32[[2],[3],[4]]
    #   p Numo::NArray.hstack([a,b])
    #   # Numo::Int32#shape=[3,2]
    #   # [[1, 2],
    #   #  [2, 3],
    #   #  [3, 4]]

    def hstack(arrays)
      klass = (self==NArray) ? NArray.array_type(arrays) : self
      nd = 0
      arys = arrays.map do |a|
        a = klass.cast(a)
        nd = a.ndim if a.ndim > nd
        a
      end
      dim = (nd >= 2) ? 1 : 0
      concatenate(arys,axis:dim)
    end

    # Stack arrays in depth wise (along third axis).
    # @example
    #   a = Numo::Int32[1,2,3]
    #   b = Numo::Int32[2,3,4]
    #   p Numo::NArray.dstack([a,b])
    #   # Numo::Int32#shape=[1,3,2]
    #   # [[[1, 2],
    #   #   [2, 3],
    #   #   [3, 4]]]
    #
    #   a = Numo::Int32[[1],[2],[3]]
    #   b = Numo::Int32[[2],[3],[4]]
    #   p Numo::NArray.dstack([a,b])
    #   # Numo::Int32#shape=[3,1,2]
    #   # [[[1, 2]],
    #   #  [[2, 3]],
    #   #  [[3, 4]]]

    def dstack(arrays)
      arys = arrays.map do |a|
        _atleast_3d(cast(a))
      end
      concatenate(arys,axis:2)
    end

    # Stack 1-d arrays into columns of a 2-d array.
    # @example
    #   x = Numo::Int32[1,2,3]
    #   y = Numo::Int32[2,3,4]
    #   p Numo::NArray.column_stack([x,y])
    #   # Numo::Int32#shape=[3,2]
    #   # [[1, 2],
    #   #  [2, 3],
    #   #  [3, 4]]

    def column_stack(arrays)
      arys = arrays.map do |a|
        a = cast(a)
        case a.ndim
        when 0; a[:new,:new]
        when 1; a[true,:new]
        else; a
        end
      end
      concatenate(arys,axis:1)
    end

    private
    # Return an narray with at least two dimension.
    def _atleast_2d(a)
      case a.ndim
      when 0; a[:new,:new]
      when 1; a[:new,true]
      else;   a
      end
    end

    # Return an narray with at least three dimension.
    def _atleast_3d(a)
      case a.ndim
      when 0; a[:new,:new,:new]
      when 1; a[:new,true,:new]
      when 2; a[true,true,:new]
      else;   a
      end
    end

    end # class << self

    # @example
    #   p a = Numo::DFloat[[1, 2], [3, 4]]
    #   # Numo::DFloat#shape=[2,2]
    #   # [[1, 2],
    #   #  [3, 4]]
    #
    #   p b = Numo::DFloat[[5, 6]]
    #   # Numo::DFloat#shape=[1,2]
    #   # [[5, 6]]
    #
    #   p a.concatenate(b,axis:0)
    #   # Numo::DFloat#shape=[3,2]
    #   # [[1, 2],
    #   #  [3, 4],
    #   #  [5, 6]]
    #
    #   p a.concatenate(b.transpose, axis:1)
    #   # Numo::DFloat#shape=[2,3]
    #   # [[1, 2, 5],
    #   #  [3, 4, 6]]

    def concatenate(*arrays,axis:0)
      axis = check_axis(axis)
      self_shape = shape
      self_shape.delete_at(axis)
      sum_size = shape[axis]
      arrays.map! do |a|
        case a
        when NArray
          # ok
        when Numeric
          a = self.class.new(1).store(a)
        when Array
          a = self.class.cast(a)
        else
          raise TypeError,"not Numo::NArray: #{a.inspect[0..48]}"
        end
        if a.ndim > ndim
          raise ShapeError,"dimension mismatch"
        end
        a_shape = a.shape
        sum_size += a_shape.delete_at(axis-ndim) || 1
        if self_shape != a_shape
          raise ShapeError,"shape mismatch"
        end
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
        lst = fst + (a.shape[axis-ndim] || 1)
        refs[axis] = fst...lst
        result[*refs] = a
      end
      result
    end

    # @example
    #   p x = Numo::DFloat.new(9).seq
    #   # Numo::DFloat#shape=[9]
    #   # [0, 1, 2, 3, 4, 5, 6, 7, 8]
    #
    #   pp x.split(3)
    #   # [Numo::DFloat(view)#shape=[3]
    #   # [0, 1, 2],
    #   #  Numo::DFloat(view)#shape=[3]
    #   # [3, 4, 5],
    #   #  Numo::DFloat(view)#shape=[3]
    #   # [6, 7, 8]]
    #
    #   p x = Numo::DFloat.new(8).seq
    #   # Numo::DFloat#shape=[8]
    #   # [0, 1, 2, 3, 4, 5, 6, 7]
    #
    #   pp x.split([3, 5, 6, 10])
    #   # [Numo::DFloat(view)#shape=[3]
    #   # [0, 1, 2],
    #   #  Numo::DFloat(view)#shape=[2]
    #   # [3, 4],
    #   #  Numo::DFloat(view)#shape=[1]
    #   # [5],
    #   #  Numo::DFloat(view)#shape=[2]
    #   # [6, 7],
    #   #  Numo::DFloat(view)#shape=[0][]]

    def split(indices_or_sections, axis:0)
      axis = check_axis(axis)
      size_axis = shape[axis]
      case indices_or_sections
      when Integer
        div_axis, mod_axis = size_axis.divmod(indices_or_sections)
        refs = [true]*ndim
        beg_idx = 0
        mod_axis.times.map do |i|
          end_idx = beg_idx + div_axis + 1
          refs[axis] = beg_idx ... end_idx
          beg_idx = end_idx
          self[*refs]
        end +
        (indices_or_sections-mod_axis).times.map do |i|
          end_idx = beg_idx + div_axis
          refs[axis] = beg_idx ... end_idx
          beg_idx = end_idx
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

    # @example
    #   p x = Numo::DFloat.new(4,4).seq
    #   # Numo::DFloat#shape=[4,4]
    #   # [[0, 1, 2, 3],
    #   #  [4, 5, 6, 7],
    #   #  [8, 9, 10, 11],
    #   #  [12, 13, 14, 15]]
    #
    #   pp x.hsplit(2)
    #   # [Numo::DFloat(view)#shape=[4,2]
    #   # [[0, 1],
    #   #  [4, 5],
    #   #  [8, 9],
    #   #  [12, 13]],
    #   #  Numo::DFloat(view)#shape=[4,2]
    #   # [[2, 3],
    #   #  [6, 7],
    #   #  [10, 11],
    #   #  [14, 15]]]
    #
    #   pp x.hsplit([3, 6])
    #   # [Numo::DFloat(view)#shape=[4,3]
    #   # [[0, 1, 2],
    #   #  [4, 5, 6],
    #   #  [8, 9, 10],
    #   #  [12, 13, 14]],
    #   #  Numo::DFloat(view)#shape=[4,1]
    #   # [[3],
    #   #  [7],
    #   #  [11],
    #   #  [15]],
    #   #  Numo::DFloat(view)#shape=[4,0][]]

    def vsplit(indices_or_sections)
      split(indices_or_sections, axis:0)
    end

    def hsplit(indices_or_sections)
      split(indices_or_sections, axis:1)
    end

    def dsplit(indices_or_sections)
      split(indices_or_sections, axis:2)
    end

    # @example
    #   p a = Numo::NArray[0,1,2]
    #   # Numo::Int32#shape=[3]
    #   # [0, 1, 2]
    #
    #   p a.tile(2)
    #   # Numo::Int32#shape=[6]
    #   # [0, 1, 2, 0, 1, 2]
    #
    #   p a.tile(2,2)
    #   # Numo::Int32#shape=[2,6]
    #   # [[0, 1, 2, 0, 1, 2],
    #   #  [0, 1, 2, 0, 1, 2]]
    #
    #   p a.tile(2,1,2)
    #   # Numo::Int32#shape=[2,1,6]
    #   # [[[0, 1, 2, 0, 1, 2]],
    #   #  [[0, 1, 2, 0, 1, 2]]]
    #
    #   p b = Numo::NArray[[1, 2], [3, 4]]
    #   # Numo::Int32#shape=[2,2]
    #   # [[1, 2],
    #   #  [3, 4]]
    #
    #   p b.tile(2)
    #   # Numo::Int32#shape=[2,4]
    #   # [[1, 2, 1, 2],
    #   #  [3, 4, 3, 4]]
    #
    #   p b.tile(2,1)
    #   # Numo::Int32#shape=[4,2]
    #   # [[1, 2],
    #   #  [3, 4],
    #   #  [1, 2],
    #   #  [3, 4]]
    #
    #   p c = Numo::NArray[1,2,3,4]
    #   # Numo::Int32#shape=[4]
    #   # [1, 2, 3, 4]
    #
    #   p c.tile(4,1)
    #   # Numo::Int32#shape=[4,4]
    #   # [[1, 2, 3, 4],
    #   #  [1, 2, 3, 4],
    #   #  [1, 2, 3, 4],
    #   #  [1, 2, 3, 4]]

    def tile(*arg)
      arg.each do |i|
        if !i.kind_of?(Integer) || i<1
          raise ArgumentError,"argument should be positive integer"
        end
      end
      ns = arg.size
      nd = self.ndim
      shp = self.shape
      new_shp = []
      src_shp = []
      res_shp = []
      (nd-ns).times do
        new_shp << 1
        new_shp << (n = shp.shift)
        src_shp << :new
        src_shp << true
        res_shp << n
      end
      (ns-nd).times do
        new_shp << (m = arg.shift)
        new_shp << 1
        src_shp << :new
        src_shp << :new
        res_shp << m
      end
      [nd,ns].min.times do
        new_shp << (m = arg.shift)
        new_shp << (n = shp.shift)
        src_shp << :new
        src_shp << true
        res_shp << n*m
      end
      self.class.new(*new_shp).store(self[*src_shp]).reshape(*res_shp)
    end

    # @example
    #   p Numo::NArray[3].repeat(4)
    #   # Numo::Int32#shape=[4]
    #   # [3, 3, 3, 3]
    #
    #   p x = Numo::NArray[[1,2],[3,4]]
    #   # Numo::Int32#shape=[2,2]
    #   # [[1, 2],
    #   #  [3, 4]]
    #
    #   p x.repeat(2)
    #   # Numo::Int32#shape=[8]
    #   # [1, 1, 2, 2, 3, 3, 4, 4]
    #
    #   p x.repeat(3,axis:1)
    #   # Numo::Int32#shape=[2,6]
    #   # [[1, 1, 1, 2, 2, 2],
    #   #  [3, 3, 3, 4, 4, 4]]
    #
    #   p x.repeat([1,2],axis:0)
    #   # Numo::Int32#shape=[3,2]
    #   # [[1, 2],
    #   #  [3, 4],
    #   #  [3, 4]]

    def repeat(arg,axis:nil)
      case axis
      when Integer
        axis = check_axis(axis)
        c = self
      when NilClass
        c = self.flatten
        axis = 0
      else
        raise ArgumentError,"invalid axis"
      end
      case arg
      when Integer
        if !arg.kind_of?(Integer) || arg<1
          raise ArgumentError,"argument should be positive integer"
        end
        idx = c.shape[axis].times.map{|i| [i]*arg}.flatten
      else
        arg = arg.to_a
        if arg.size != c.shape[axis]
          raise ArgumentError,"repeat size shoud be equal to size along axis"
        end
        arg.each do |i|
          if !i.kind_of?(Integer) || i<0
            raise ArgumentError,"argument should be non-negative integer"
          end
        end
        idx = arg.each_with_index.map{|a,i| [i]*a}.flatten
      end
      ref = [true] * c.ndim
      ref[axis] = idx
      c[*ref].copy
    end

    # Calculate the n-th discrete difference along given axis.
    # @example
    #   p x = Numo::DFloat[1, 2, 4, 7, 0]
    #   # Numo::DFloat#shape=[5]
    #   # [1, 2, 4, 7, 0]
    #
    #   p x.diff
    #   # Numo::DFloat#shape=[4]
    #   # [1, 2, 3, -7]
    #
    #   p x.diff(2)
    #   # Numo::DFloat#shape=[3]
    #   # [1, 1, -10]
    #
    #   p x = Numo::DFloat[[1, 3, 6, 10], [0, 5, 6, 8]]
    #   # Numo::DFloat#shape=[2,4]
    #   # [[1, 3, 6, 10],
    #   #  [0, 5, 6, 8]]
    #
    #   p x.diff
    #   # Numo::DFloat#shape=[2,3]
    #   # [[2, 3, 4],
    #   #  [5, 1, 2]]
    #
    #   p x.diff(axis:0)
    #   # Numo::DFloat#shape=[1,4]
    #   # [[-1, 2, 0, -2]]

    def diff(n=1,axis:-1)
      axis = check_axis(axis)
      if n < 0 || n >= shape[axis]
        raise ShapeError,"n=#{n} is invalid for shape[#{axis}]=#{shape[axis]}"
      end
      # calculate polynomial coefficient
      c = self.class[-1,1]
      2.upto(n) do |i|
        x = self.class.zeros(i+1)
        x[0..-2] = c
        y = self.class.zeros(i+1)
        y[1..-1] = c
        c = y - x
      end
      s = [true]*ndim
      s[axis] = n..-1
      result = self[*s].dup
      sum = result.inplace
      (n-1).downto(0) do |i|
        s = [true]*ndim
        s[axis] = i..-n-1+i
        sum + self[*s] * c[i] # inplace addition
      end
      return result
    end

    private
    def check_axis(axis)
      if axis < 0
        axis += ndim
      end
      if axis < 0 || axis >= ndim
        raise ArgumentError,"invalid axis"
      end
      axis
    end

  end
end
