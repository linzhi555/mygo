# myGo

myGo is a language for distributed system.


synatax

``` go
var localIP = GetLocalIP();
var remoteIP = FindRemoteWokerIP();

var WorkFinished@localIP = 0;

mygo@remoteIP func(){
    for var i =1;i<10;i++ {
        WorkFinished@localIP ++;
        sleep(1);
    }
}()

for WorkFinished@localIP < 10 {
    sleep(1)
}

```
