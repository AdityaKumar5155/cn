# Simulated Token Ring Topology in NS2

# Create simulator
set ns [new Simulator]

# Trace file for NAM
set nf [open out.nam w]
$ns namtrace-all $nf

# Create 4 nodes in a ring
for {set i 0} {$i < 4} {incr i} {
    set n($i) [$ns node]
}

# Connect nodes in a ring
for {set i 0} {$i < 4} {incr i} {
    set next [expr ($i + 1) % 4]
    $ns duplex-link $n($i) $n($next) 1Mb 10ms DropTail
}

# Setup token passing simulation using UDP and timers

# Create UDP agents and Null sinks
for {set i 0} {$i < 4} {incr i} {
    set udp($i) [new Agent/UDP]
    set null($i) [new Agent/Null]
    $ns attach-agent $n($i) $udp($i)
    $ns attach-agent $n([expr ($i + 1) % 4]) $null($i)
    $ns connect $udp($i) $null($i)

    set cbr($i) [new Application/Traffic/CBR]
    $cbr($i) set packetSize_ 500
    $cbr($i) set interval_ 1.0
    $cbr($i) attach-agent $udp($i)
}

# Simulate token passing: each node sends data only at its turn
$ns at 1.0 "$cbr(0) start"
$ns at 1.5 "$cbr(0) stop; $cbr(1) start"
$ns at 2.0 "$cbr(1) stop; $cbr(2) start"
$ns at 2.5 "$cbr(2) stop; $cbr(3) start"
$ns at 3.0 "$cbr(3) stop; $cbr(0) start"
$ns at 3.5 "$cbr(0) stop"

# Finish the simulation
$ns at 4.0 "finish"

proc finish {} {
    global ns nf
    $ns flush-trace
    close $nf
    exec nam out.nam &
    exit 0
}

# Run simulation
$ns run
