// String Methods
const __to_n                 = Charly.internals.get_method("String::to_n")

// Buffer Methods
const __buffer_create        = Charly.internals.get_method("Buffer::create")
const __buffer_reserve       = Charly.internals.get_method("Buffer::reserve")
const __buffer_seek          = Charly.internals.get_method("Buffer::seek")
const __buffer_write         = Charly.internals.get_method("Buffer::write")
const __buffer_set           = Charly.internals.get_method("Buffer::set")
const __buffer_create_string = Charly.internals.get_method("Buffer::create_string")
const __buffer_destroy       = Charly.internals.get_method("Buffer::destroy")

export = ->(Base) {
  return class String extends Base {

    /*
     * Checks if this string is a digit
     *
     * ```
     * "5".is_digit() // => true
     * "2".is_digit() // => true
     * "0".is_digit() // => true
     * "f".is_digit() // => false
     * ```
     * */
    func is_digit {
      const r = ({
        @"0": true,
        @"1": true,
        @"2": true,
        @"3": true,
        @"4": true,
        @"5": true,
        @"6": true,
        @"7": true,
        @"8": true,
        @"9": true
      })[self]
      r || false
    }

    /*
     * Convert this string into a number
     *
     * ```
     * "2562".to_n() // => 2562
     * "-2562".to_n() // => -2562
     * "25.5".to_n() // => 25.5
     * "-1.88".to_n() // => -1.88
     * "test".to_n() // => NaN
     * ```
     * */
    func to_n {
      if self.length == 0 return NaN
      if self == " " return NaN
      __to_n(self)
    }
  }
}
