#ifndef ARRAY_H
#define ARRAY_H

template <typename T>
class BasicArray
{
private:
  T *_array;
  int _capacity = 8;
  int _size;
public:
  BasicArray(int c = 8)
  {
    _capacity = c;
    _array = new T[_capacity];
    _size = 0;
  }

  void push_back(T element)
  {
    if (size() + 1 > _capacity)
    {
      _capacity *= 2;
      T *newArray = new T[_capacity];

      memcpy(newArray, _array, sizeof(T) * _size);
      delete _array;
      _array = newArray;
    }
    _array[_size++] = element;
  }
  void clear()
  {
    _size = 0;  
  }

  void remove()
  {
    if(_size>0){
      _size--;  
    }    
  }
  int size()
  {
    return _size;
  }

  void positionGuard(int position){
    if (position < 0 || position >= _size)
    {
      throw std::out_of_range("Out of bound array access");
    }
  }

  T get(int position)
  {
    positionGuard(position);
    return _array[position];
  }

  void set(int position, T value)
  {
    positionGuard(position);
    _array[position] = value;
  }
};

#endif // ARRAY_H