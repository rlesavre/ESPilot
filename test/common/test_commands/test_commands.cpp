#define DOCTEST_CONFIG_IMPLEMENT // REQUIRED: Enable custom main()
#include <doctest.h>
#include <ConsoleCommand.h>

// TEST_CASE ...
TEST_CASE("command can be created")
{
  bool commandCalled = false;
  const char *argumentsSaved = nullptr;
  ConsoleCommand* command = new ConsoleCommand("hello", [&commandCalled, &argumentsSaved](const char *arguments){ 
    commandCalled = true; 
    argumentsSaved = arguments;
    return 1;
  });

  SUBCASE("detect bad candidate")
  {
    CHECK(command->isCandidate(NULL) == false);
    CHECK(command->isCandidate("toto") == false);
    CHECK(command->isCandidate("hell") == false);
    CHECK(command->isCandidate("helo") == false);
    CHECK(command->isCandidate("helloo world") == false);
    CHECK(command->isCandidate(" hello") == false);
  }

  SUBCASE("detect good candidate")
  {
    CHECK(command->isCandidate("hello"));
    CHECK(command->isCandidate("HELLO"));
    CHECK(command->isCandidate("Hello"));
    CHECK(command->isCandidate("hello world"));
  }

  SUBCASE("call callback")
  {
    commandCalled = false;
    argumentsSaved = nullptr;
    command->execute("hello", 5);
    CHECK(commandCalled);
    CHECK(argumentsSaved == nullptr);
  }

  SUBCASE("call callback with correct argument")
  {
    commandCalled = false;
    argumentsSaved = nullptr;
    command->execute("hello world", 11);
    CHECK(commandCalled);
    CHECK(argumentsSaved != nullptr);
    CHECK(strncmp(argumentsSaved, "world", 5) == 0);
  }
  delete command;
}


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