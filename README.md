# myGo

myGo is a language for distributed system.


Go is great language, The two most beautiful feature is Channel an 'go' keyword.

MyGo bring these two feature to Distributed System.

Except we can use _go_ to start a goroutine in local machine,
we can use _mygo_ key word to start a gorotine in remote machine.
Then we can commnuicate with them with channel.


``` go
var localIP = GetLocalIP();
var remoteIP = FindRemoteWokerIP();

var WorkChan@localIP = make(chan[int])

mygo@remoteIP func(){
    for var i =1;i<10;i++ {
        WorkChan@localIP <- i
        sleep(1)
    }
}()

for {
    i := <- workChan@localIP
    fmt.Printf("work {} is finished\n",i)
    if i == 9 {
        break
    }
}

```
