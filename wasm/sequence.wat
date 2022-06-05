(module 
    (import "env" "mem" (memory 8192))

    (func (export "run_with_x")
        (param $reps i32)
        (local $entry0 i32)
        (local $entry1 i32)
        (local $entry2 i32)
        (local $entry3 i32)
        (local $entryX i32)
        (local $clear0 i32)
        (local $clear1 i32)
        (local $clear2 i32)
        (local $clear3 i32)
        (local $i i32)

        i32.const 0x01000
        local.set $entry0
        i32.const 0x11040
        local.set $entry1
        i32.const 0x21080
        local.set $entry2
        i32.const 0x310c0
        local.set $entry3
        i32.const 0x41100
        local.set $entryX
        i32.const 0x51140
        local.set $clear0
        i32.const 0x61140
        local.set $clear1
        i32.const 0x71180
        local.set $clear2
        i32.const 0x811c0
        local.set $clear3

        i32.const 0
        local.get $clear0
        i32.add
        i32.load
        local.get $clear1
        i32.add
        i32.load
        local.get $clear2
        i32.add
        i32.load
        local.get $clear3
        i32.add
        i32.load

        local.get $entry0
        i32.add
        i32.load
        local.get $entry1
        i32.add
        i32.load
        local.get $entry2
        i32.add
        i32.load
        local.get $entry3
        i32.add
        i32.load
        local.get $entryX
        i32.add
        i32.load

        local.get $entry3
        i32.add
        i32.load
        local.get $entry1
        i32.add
        i32.load
        local.get $entry0
        i32.add
        i32.load
        local.get $entry2
        i32.add
        i32.load

        loop $forloop (param i32) (result i32)
            local.get $entry0
            i32.add
            i32.load
            local.get $entry3
            i32.add
            i32.load
            local.get $entry0
            i32.add
            i32.load
            local.get $entry1
            i32.add
            i32.load

            local.get $entry0
            i32.add
            i32.load
            local.get $entry2
            i32.add
            i32.load
            local.get $entry0
            i32.add
            i32.load
            local.get $entry3
            i32.add
            i32.load

            local.get $entry0
            i32.add
            i32.load
            local.get $entry1
            i32.add
            i32.load
            local.get $entry0
            i32.add
            i32.load
            local.get $entry2
            i32.add
            i32.load

            local.get $i
            i32.const 1
            i32.add
            local.set $i
            local.get $i
            local.get $reps
            i32.le_s
            br_if $forloop
        end
        drop
    )

    (func (export "run_without_x")
        (param $reps i32)
        (local $entry0 i32)
        (local $entry1 i32)
        (local $entry2 i32)
        (local $entry3 i32)
        (local $entryX i32)
        (local $clear0 i32)
        (local $clear1 i32)
        (local $clear2 i32)
        (local $clear3 i32)
        (local $i i32)

        i32.const 0x01000
        local.set $entry0
        i32.const 0x11040
        local.set $entry1
        i32.const 0x21080
        local.set $entry2
        i32.const 0x310c0
        local.set $entry3
        i32.const 0x41100
        local.set $entryX
        i32.const 0x51140
        local.set $clear0
        i32.const 0x61140
        local.set $clear1
        i32.const 0x71180
        local.set $clear2
        i32.const 0x811c0
        local.set $clear3
        
        i32.const 0
        local.get $clear0
        i32.add
        i32.load
        local.get $clear1
        i32.add
        i32.load
        local.get $clear2
        i32.add
        i32.load
        local.get $clear3
        i32.add
        i32.load

        local.get $entry0
        i32.add
        i32.load
        local.get $entry1
        i32.add
        i32.load
        local.get $entry2
        i32.add
        i32.load
        local.get $entry3
        i32.add
        i32.load

        local.get $entry3
        i32.add
        i32.load
        local.get $entry1
        i32.add
        i32.load
        local.get $entry0
        i32.add
        i32.load
        local.get $entry2
        i32.add
        i32.load

        loop $forloop (param i32) (result i32)
            local.get $entry0
            i32.add
            i32.load
            local.get $entry3
            i32.add
            i32.load
            local.get $entry0
            i32.add
            i32.load
            local.get $entry1
            i32.add
            i32.load

            local.get $entry0
            i32.add
            i32.load
            local.get $entry2
            i32.add
            i32.load
            local.get $entry0
            i32.add
            i32.load
            local.get $entry3
            i32.add
            i32.load

            local.get $entry0
            i32.add
            i32.load
            local.get $entry1
            i32.add
            i32.load
            local.get $entry0
            i32.add
            i32.load
            local.get $entry2
            i32.add
            i32.load

            local.get $i
            i32.const 1
            i32.add
            local.set $i
            local.get $i
            local.get $reps
            i32.le_s
            br_if $forloop
        end
        drop
    )
)
