export = class ResultPrinter {
  display(results) {
    const status = results.passed()

    if status {
      print("All tests have passed")
    } else {
      print("Some test suites have failed")

      const failed_tests = {}

      let index = 1
      results.deep_failed(->(nodes) {

        // Extract the title of the failed test case
        const title = nodes.filter(->$1 > 0)
                            .map(->$0.title)
                            .join(" ")
        const fnode = nodes.last()

        write(index + ") ")
        print(title)

        print("Assertion #" + (fnode.index + 1))

        // Check if the types match
        print("Expected value: " + fnode.expected.to_s())
        if fnode.real.is_a(Error) && ARGV.contains("--hide-stacktraces") {
          print("Actual value:   " + fnode.real.message)
        } else {
          print("Actual value:   " + fnode.real.to_s())
        }

        write("\n")

        index += 1
      })
    }
  }
}
