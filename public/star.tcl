# Star Topology with UDP and TCP in NS2

# Create simulator object
set ns [new Simulator]

# Open NAM trace file
set nf [open out.nam w]
$ns namtrace-all $nf

# Create central node (hub) and 4 peripheral nodes (spokes)
set hub [$ns node]
for {set i 0} {$i < 4} {incr i} {
    set n($i) [$ns node]
}

# Connect each peripheral node to the hub
for {set i 0} {$i < 4} {incr i} {
    $ns duplex-link $n($i) $hub 1Mb 10ms DropTail
}

# --- UDP Flow from Node 0 to Node 2 (via hub) ---
set udp [new Agent/UDP]
$ns attach-agent $n(0) $udp
set null [new Agent/Null]
$ns attach-agent $n(2) $null
$ns connect $udp $null

set cbr [new Application/Traffic/CBR]
$cbr set packetSize_ 500
$cbr set interval_ 0.5
$cbr attach-agent $udp

# --- TCP Flow from Node 1 to Node 3 (via hub) ---
set tcp [new Agent/TCP]
$tcp set class_ 2
$ns attach-agent $n(1) $tcp

set sink [new Agent/TCPSink]
$ns attach-agent $n(3) $sink
$ns connect $tcp $sink

set ftp [new Application/FTP]
$ftp attach-agent $tcp

# Start and stop traffic
$ns at 1.0 "$cbr start"
$ns at 1.5 "$ftp start"
$ns at 4.0 "$cbr stop"
$ns at 4.5 "$ftp stop"
$ns at 5.0 "finish"

# Finish procedure
proc finish {} {
    global ns nf
    $ns flush-trace
    close $nf
    exec nam out.nam &
    exit 0
}

# Run the simulation
$ns run
