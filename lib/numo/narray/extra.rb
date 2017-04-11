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

    def self.concatenate(arrays,axis:0)
      klass = (self==NArray) ? NArray.array_type(arrays) : self
      nd = 0
      arrays.map! do |a|
        case a
        when NArray
          # ok
        when Numeric
          a = klass.new(1).store(a)
        when Array
          a = klass.cast(a)
        else
          raise TypeError,"not Numo::NArray"
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

    def self.vstack(arrays)
      self.concatenate(arrays,axis:0)
    end

    def self.hstack(arrays)
      self.concatenate(arrays,axis:1)
    end

    def self.dstack(arrays)
      self.concatenate(arrays,axis:2)
    end

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
          raise TypeError,"not Numo::NArray"
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
