# Token Ring Topology with TCP and CBR over 6 Nodes

# Create simulator object
set ns [new Simulator]

# Open the NAM trace file
set nf [open out.nam w]
$ns namtrace-all $nf

# Create 6 nodes
for {set i 0} {$i < 6} {incr i} {
    set n($i) [$ns node]
}

# Create duplex links to form a ring topology
for {set i 0} {$i < 6} {incr i} {
    set next [expr ($i + 1) % 6]
    $ns duplex-link $n($i) $n($next) 1Mb 10ms DropTail
}

# Create TCP Agent on Node 1 (source)
set tcp [new Agent/TCP]
$tcp set class_ 2
$ns attach-agent $n(1) $tcp

# Create TCP Sink Agent on Node 4 (destination)
set sink [new Agent/TCPSink]
$ns attach-agent $n(4) $sink

# Connect TCP source to sink
$ns connect $tcp $sink

# Create CBR traffic over TCP
set app [new Application/Traffic/CBR]
$app set packetSize_ 500
$app set interval_ 0.2
$app attach-agent $tcp

# Schedule events
$ns at 1.0 "$app start"
$ns at 4.0 "$app stop"
$ns at 5.0 "finish"

# Finish procedure
proc finish {} {
    global ns nf
    $ns flush-trace
    close $nf
    exec nam out.nam &
    exit 0
}

# Run simulation
$ns run
