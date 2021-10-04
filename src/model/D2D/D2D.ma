[top]
components : Due@due  relaySYSTEM
out : out_port
link : out@due in@relaySYSTEM
link : out@relaySYSTEM out_port

[relaySYSTEM]
components : Relay1@relay Relay2@relay Relay3@relay Relay4@relay controller
out : out stats
in : in   
Link :  controller@relay1Out packetPort@Relay1
Link :  controller@relay2Out packetPort@Relay2
Link :  controller@relay3Out packetPort@Relay3
Link :  controller@relay4Out packetPort@Relay4
Link :  protocol@Relay1 controller@relay1In
Link :  protocol@Relay2 controller@relay2In  
Link :  protocol@Relay3 controller@relay3In  
Link :  protocol@Relay4 controller@relay4In
Link :  updatePort@Relay1 controller@protocolPort
Link :  updatePort@Relay2 controller@protocolPort  
Link :  updatePort@Relay3 controller@protocolPort  
Link :  updatePort@Relay4 controller@protocolPort  
Link : in in@controller
Link : hop@Relay1 out
Link : hop@Relay2 out
Link : hop@Relay3 out
Link : hop@Relay4 out
Link : trow@controller stats

[controller]
componentes : fifo@Queue Selector@scheduler
out : relay1Out relay2Out relay3Out relay4Out trow
in :  in relayIn protocolPort
Link : out@fifo queuePort@Selector
Link : ack@Selector done@fifo
Link : relay1Out@Selector relay1Out
Link : relay2Out@Selector relay2Out
Link : relay3Out@Selector relay3Out
Link : relay4Out@Selector relay4Out
Link : relayIn relay1In@Selector 
Link : relayIn relay2In@Selector
Link : relayIn relay3In@Selector
Link : relayIn relay4In@Selector
Link : protocolPort@Selector protocolPort
Link : trow@Selector trow
Link : in in@fifo



[relay]
components : Transmitter@transmitter CycleDuty@cycleScheduler
out : hop protocol updatePort 
in : protocolPort packetPort  
Link : protocolPort  protocolPort@CycleDuty
Link : packetPort packetPort@Transmitter
Link : hop@Transmitter hop
Link : updatePort@CycleDuty updatePort
Link : protocol@Transmitter protocol


[Due]
lambda : 1.2

[Transmitter]
mu : 1.5
r : 10
size : 4
l: 2
noise : 50
alpha : 1.3
Px : 13
device_density : 500
tau : 10

[CycleDuty]
c : 0.2

[scheduler]
N : 4
