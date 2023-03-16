#define DOCTEST_CONFIG_IMPLEMENT // REQUIRED: Enable custom main()
#include <doctest.h>
#include <SimpleArray.hpp>

// TEST_CASE ...
TEST_CASE("array can be sized and resized")
{
  BasicArray<int> numbers;

  REQUIRE(numbers.size() == 0);

  SUBCASE("adding to the array increases its size")
  {
    numbers.push_back(1);
    CHECK(numbers.size() == 1);
  }
  SUBCASE("removing to the array decrease its size")
  {
    numbers.remove();
    CHECK(numbers.size() == 0);
  }
  SUBCASE("adding to the array increases its size")
  {
    numbers.push_back(1);
    CHECK(numbers.size() == 1);
  }
  SUBCASE("removing to the array decrease its size")
  {
    numbers.remove();
    CHECK(numbers.size() == 0);
  }
}

TEST_CASE("array can be populated")
{
  BasicArray<int> numbers;
  const int size = 16;

  REQUIRE(numbers.size() == 0);
  for (int i = 0; i < size; i++)
  {
    numbers.push_back(i);
  }
  REQUIRE(numbers.size() == size);

  SUBCASE("populating increase size & capacity")
  {
    CHECK(numbers.size() == size);
    for (int i = 0; i < size; i++)
    {
      CHECK(numbers.get(i) == i);
    }
  }

  SUBCASE("content is correctly populated")
  {
    for (int i = 0; i < 4; i++)
    {
      numbers.remove();
    }
    for (int i = 0; i < size - 4; i++)
    {
      CHECK(numbers.get(i) == i);
    }
  }

  SUBCASE("content can be cleared")
  {
    numbers.clear();
    CHECK(numbers.size() == 0);
    for (int i = 0; i < size; i++)
    {
      numbers.push_back(i);
    }
    CHECK(numbers.size() == size);
  }

  SUBCASE("out of bound throws")
  {
    bool hasThrown = false;
    try{
      numbers.get(-1);
    } catch(std::exception ex){
      hasThrown = true;
    }
    CHECK(hasThrown);
  }
}

// TEST_SUITE ...

int main(int argc, char **argv)
{
  doctest::Context context;

  // BEGIN:: PLATFORMIO REQUIRED OPTIONS
  context.setOption("success", true);     // Report successful tests
  context.setOption("no-exitcode", true); // Do not return non-zero code on failed test case
  // END:: PLATFORMIO REQUIRED OPTIONS

  // YOUR CUSTOM DOCTEST OPTIONS

  context.applyCommandLine(argc, argv);
  return context.run();
}