# myGo



## what is myGo?
myGo is a language for distributed system.


Go is great language, The two most beautiful feature is Channel and 'go' keyword.

MyGo bring these two feature to Distributed System.

Except we can use go keyword to start a goroutine in local machine,
we can also use mygo keyword to start a gorotine in remote machine.
Then we can commnuicate with these goroutine with channel.

below is a simple example:

``` go
// get local node ip and find a ip of remote node
var localIP = GetLocalIP();
var remoteIP = FindRemoteWokerIP();

// make a channel in local node
var WorkChan@localIP = make(chan[int])

// start a goroutine in remote node
// the remote can push data to channel in local node
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

## why myGo?

Without myGo, we need  deploy a program in the local node and remote node at the same time then two node start to communicate with each other.

With myGo, we can just start the program in the local node, and then the program will be sent to remote node, program is also a kind of data!



## how to build?

### build mygo binary
```
cmake -B build
cmake --build build --target mygo
```

### build mygo binary and test binaries

```
cmake -B build
cmake --build build
```

### run exmaple

```
${buildDir}/mygo examle/***.mgo

```

### run test

``` 
${buildDir}/test_xxx

```
