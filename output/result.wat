;; WebAssembly Text Format (.wat)
;; Transpiled from Invented DSL
(module
  (import "env" "print_num" (func $print_num (param f64)))
  (func (export "main")
    (local $total f64)
    (local $greeting f64)
    f64.const 10
    f64.const 5
    f64.const 2
    f64.mul
    f64.add
    local.set $total
    local.get $total
    f64.const 15
    f64.gt
    f64.const 0.0
    f64.ne
    if
      local.get $total
      call $print_num
    else
      f64.const 0
      call $print_num
    end
    block $break_loop
      loop $continue_loop
        local.get $total
        f64.const 15
        f64.gt
        f64.const 0.0
        f64.eq
        br_if $break_loop
        local.get $total
        f64.const 1
        f64.sub
        local.set $total
        local.get $total
        call $print_num
        br $continue_loop
      end
    end
  )
)
