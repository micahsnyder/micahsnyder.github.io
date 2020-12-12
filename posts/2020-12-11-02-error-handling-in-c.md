---
title: Error Handling in C
published_date: "2020-12-11 07:32:15 +0000"
layout: default.liquid
is_draft: false
---
# {{ page.title }}

{{ page.published_date }}

----

It is far too easy for C code to spring memory leaks due to shoddy error handling. In a larger code base poor error handling habits can result in years of agony tracking down bugs and digging out from small mountain of technical debt.

No thanks to Edsger Dijkstra's infamous 1968 letter "Go To Statement Considered Harmful", just about every C programmer has been told at one time that `goto` is "bad". This still rings true except for one use of `goto`: error handling.

In this article I'll present an overview of a few different error handling methods available to the C programmer. To illustrate, I've crated example multi-threaded application that stores named data-elements in a global growable array protected by a mutex. The function in our example adds one element to the array, and allocates more space for the array as needed.

By the end of this article, I hope you'll agree with me on the best option for error handling in C (_hint_: it's `goto`).

## Multiple `return`'s

Perhaps the most common method for error handling, especially in older C code involves the use of multiple `return` statements. Allocated memory, thread locks, and other resources are cleaned up as-needed at each `return`.

Without further ado, here's our first variant of the example program featuring the "Multiple `return`s" error handling method:

<iframe frameborder="0" height="800px" src="https://godbolt.org/e?readOnly=true&hideEditorToolbars=true#z:OYLghAFBqRAWIDGB7AJgUwKKoJYBdkAnAGhxAgDNkA7PAZUQEMAbdEARgBZiArD45o2qgA%2BmMTEAzsgCuhRGwDkAegBUAajp4hqRoVTrmOAEaT1q5QFIADAEFLAJgDMOaomYyM6y04DCkvFRjZGRmADo4H0x1GPU1dQAxInVg0PMrO0cXNw8vH39A1zwIqNiY%2BKTCdQBbNHRCanUi9GB69TwATwAHdDMLG3tnV3dPdG8/ANQjYxKnaLKK5OqWZmREZQpCdDH%2BzKGc0fGCwldgWfnYxaqA/Rku9IGs4dyx/MmcZHOy8o1K9S6TrQKA9MnZ4k59ABaLp6TrqAAyJj6GUG2RGeT8XTwcC2jFQ50edk6PQwwJuMkQeG8AHYAEIDMqIOB6czqaiMaroHz0uxlABuyBwBg0um03Me1IAImyOehUCJRYwRHhxaDbPEAOroMCKZjMdQrNaMPBjbFjPSERgddTIYGK/6C2j1MyuJpuLac2hmW3qdjWawg1Gk1xjSW2AAqthEtgASjHbABNES0%2BEAeV8AGkRHQAJIALWifushNsAWNOEQ/2xuPl1RkJoAHsr1IqRBarSJVogANbjaUABXDAAkY5hbJKRABZACq4cwAA0RDmAHI58M52zw/OYGOq0vaPAVmWc%2BWtqmqDTABXGpXt60%2BaXL6fw%2BF7suHyuSHAAL3QzavrZ3iIX6/n26jFk4tJxBodA/mMPp3uoECuugrCengACUgbvkeIF/lSV7UDI1TXtoIioeg6FmA%2B4Hcj86jLsRxhtD6FHoW6BqEJa1q7PYYIXgyGgAALGCc6DArYqAGIwbLoAA7secotje6hsegtDtMg7RwGMwCrMYLCcdxYSCYJ6hCTClrVIp3zqJgaHqVS7KcmZFl6ByynaLZ/aOiaVQENpYxqRpiquVseByI0eCEDIYxlPZlGOeokgUgokiSBQMh6taeIYPiYXoBFDTqBQLCSHFMQJIwOCsAYAW5apDm0CZvK8ak%2BqMF0PTUKeN7kU1eAQCg1ABOoTIshoznoMQ6gCkKrKKlhlh0gysRTb1ZHntQ8n9YlGk0U%2BL57qtMQ4MCECHfCfY0VNNK%2BMtvgMc%2BV0PjRi00jyti2TEAJFBQECOA4uUiOtIhbJSQh6Ww6g5tQfIsPNejAMRjmSGAYCWAArL41CAxhe7feo4WRSVZVcpBJ00pKJYLBoth6kaJoGrJCnkpSmn/PWNqNLIVRGAEYSBmU21ybt7E0csDOIBAeG2hAoNnhh%2BMU7ysRnUhl3XU40oi2LjmQlEU1LStqu2fEqZcz6nK1IQHTo0L32/UCAMOEDUkg7K8rg9owisCAiTVbVHOGkwTPW0Q1pUFUIueTJwV4OjWM43jBPfcTxWlcw5WpzEy3U2qtPqFqLY0DqVLIHJUU6YpM3SIYhU6mYqB3EYodjPggu8cLO3xwbcy3TRNzN108uysrn1lOrF3PVrOs9wNfeYEbH2U4XFvl8C4e2/bXffZs2zy/Pe2YTn3xO3g/2A8DoPe5DfsBzVSn1VLxpjCgXTWj69pTYn2O46749V6xHTo0TO2cVZfViHnGmlwNBah1FsFI6BTjqGADgPk6kbSV2dHAHA9wfRmljrXLScltS6n1DwGQo1GDpRwMAKuYwui%2BXqJ3FE3dRa90NrKMCoUIGU3iPCNYvZCH8ypHXUhY0hDJUYBQVCOUpI2jkCzRqx9WGUyxDidAeIRB1kbJ2IRLsABsgEuIdi7N2QBBdYGrzpl1dSBheaxxUexAKhD7E2mBIQu8LVIGxF3qdc6mtXra1QaRW8piOjGwnt9ACfVEISxDhAMMkZoxxkTMmNMmZszblZLLf6Cs%2Bp4FUErU%2Bk9Akz2CdKWJZE7xRKAbZc%2Bl9XbX09mDdAENfbQyqo/OqWkQ6vxqJRCOJVkj2jvL/ZOADjqm0JiVLY6BD4cIXlwzkljfGzP3gs3W8c1mzOAYVEmYDybRNstAmZZRqnhO4sBOCYFklRljPGJMKZ0xZlzAWU%2BedGrlSaOdQixEwl6y9LPUJJjrl4Tqec2B6hbARKaGYTKeoZr9LDkQMYW8eJsO%2BgUza5gNAmmqF0aZ6yygEvuDRXEUtoCArvDNalYKOx4W8A4KC9zUlPIya87JBYsIaDyaPE8gKiklIgYTKeQTtY0TJZCkljtAQXxdm7eUN92k%2Byhv7bpQcArDFxD8plX8VLjIxn/FOfCoW2U2YsoFeBF5G1KXveZVqdn2tsiA0mWdjn1Kpl6y5bY4VSsokS0VMSaURJuaBRw9IQlsseekl5WT3mYE%2BVKGBMRfV3ixrSf5JFWzx0kFjaUN0j7oVPtmwFebmVRulOwYlhdpzUHMYFQwOBRp12QGafymi8RmCYI0RgiA0ounLm4dFaK1EzI0TWHR9Z0BNhkA2gxjhjFxLDeYtZlM3XRVisdFNdhFAYQECARQmNFDEGoEe6wp7kBHsetIOQChmVOHYKevAF790Hu7Bwf0h7FCcFPeexQl7iDXsUKeyQIBrDEFfYB99xAMGEC/DQEAnAgA%3D%3D%3D"></iframe>

As you review the above example, note how much duplicate code exists for resource cleanup during error handling. Can you see how easy it would be to forget to clean up a malloced pointer or a locked mutex. This is a recipe for disaster!

## The `do { ... } while(0)`

Another surprisingly popular option for error handling in C is to wrap all function code in a big ol' `do { ... } while(0);` "loop".

I'm not a big fan of the `do { ... } while(0);` approach to error handling for a few reasons...

First and foremost, it's an obvious hack to perform a `goto` without using the word `goto`!

Secondly, it's confusing. To the novice programmer, this approach for error handling is non-obvious because it abuses the do-while looping construct to perform a completely different task. An experienced programmer may appreciate such cleverness but the simple fact is that code which is hard to read is hard to maintain and increases the barrier to entry for novice programmers with little if any real benefit.

Additionally, I think the extra indentation of the  `do { ... } while(0);` is harder to read.

Here's our example using the do-while method:

<iframe frameborder="0" height="800px" src="https://godbolt.org/e?readOnly=true&hideEditorToolbars=true#z:OYLghAFBqRAWIDGB7AJgUwKKoJYBdkAnAGhxAgDNkA7PAZUQEMAbdEARgBZiArD45o2qgA%2BmMTEAzsgCuhRGwDkAegBUAajp4hqRoVTrmOAEaT1q5QFIADAEFLAJgDMOaomYyM6y04DCkvFRjZGRmADo4H0x1GPU1dQAxInVg0PMrO0cXNw8vH39A1zwIqNiY%2BKTCdQBbNHRCanUi9GB69TwATwAHdDMLG3tnV3dPdG8/ANQjYxKnaLKK5OqWZmREZQpCdDH%2BzKGc0fGCwldgWfnYxaqA/Rku9IGs4dyx/MmcZHOy8o1K9S6TrQKA9MnZ4k59ABaLp6TrqAAyJj6GUG2RGeT8XTwcC2jFQ50edk6PQwwJuMkQeG8AHYAEIDMqIOB6czqaiMaroHz0uxlABuyBwBg0um03Me1IAImyOehUCJRYwRHhxaDbPEAOroMCKZjMdQrNaMPBjbFjPSERgddTIYGK/6C2j1MyuJpuLac2hmW3qdjWawg1Gk1xjSW2AAqthEtgASjHbABNES0%2BEAeV8AGkRHQAJIALWifushNsAWNOEQ/2xuPl1RkJoAHsr1IqRBarSJVogANbjaUABXDAAkY5hbJKRABZACq4cwAA0RDmAHI58M52zw/OYGOq0vaPAVmWc%2BWtqmqDTABXGpXt60%2BaXL6fw%2BF7suHyuSHAAL3QzavrZ3iIX6/n26jFk4tILBodA/mMPp3uoECuugrCengACUgbvkeIF/lSV7UDI1TXtoIioeg6FmA%2B4Hcj86jLsRxhtD6FHoW6BqEJa1q7PYYIXgyGgAALGCc6DArYqAGIwbLoAA7secotje6hsegtDtMg7RwGMwCrMYLCcdxYSCYJ6hCTClrVIp3zqJgaHqVS7KcmZFl6ByynaLZ/aOiaVQENpYxqRpiquVseByI0eCEDIYxlPZlGOeokgUgokiSBQMh6taeIYPiYXoBFDTqBQLCSHFMQJIwOCsAYAW5apDm0CZvK8ak%2BqMF0PTUKeN7kU1eAQCg1ABOoTIshoznoMQ6gCkKrKKlhlh0gysTtclB4yNRTjSqVzDlXuZRTb1ZHntQ8n9YlGk0U%2BL57qtMSoFpy08rYtkxDgwIQLd8J9jRU00r4y2%2BAxz6/Q%2BNGLTSr3vd88S0ninHAJIYBgIGsNlKJ6CMN2h3vctkolrD8S2HqRomgaskKeSlKaf89Y2o0shVHeYTo%2B951yZd7E0csZOIBAeG2hAx2kUqmEYXjtmfUhP1/TtVPc45S0rbyGP0amDM%2BpytSEB0qPs%2BrWM41L3wE0T72amMT3UDqVLIHJUU6YpM3SIYhU6mYqB3EYTAU/gbO8bDnNK7QkJRADNE3N7XQi7KkuQQ93wy99YPy9KIfBXg4dzFNKsw%2BrcQaJr9vAjrRD62jQeF8buOJ2rtnm2qxMaFqOpbCk6CnOowA4Hy6k2o7zpwDg9w%2BmanmMK7Wlydqur6jwW1Uow6U4MATtjF0vn1IHKLBxdWc55gkcK6F9d8W9lsaPCay9hPiFu7PY1CMljAUKhOVSTachU41V3FIbb4WIcTY1rPWdATYuzdggI4AAbIBLiHYoEJ1eknaCaCygaFsF1dSBhmaTz/uxAKE9cE2mBPfRBHQWqX3etXaWX05YQwVgBPqd584YNsiTShLZkC9FtooKkECcABBmqwPAnsDT82NGMfAgYOHfBYWRRCvNDQCzDJGaMcZEzJjTJmbM25WRCwoHHE8YtlSqAwig%2BRZQU6MJ2jRRRt5KHsIboXeiVUapKXqlIim5c9YlWSPaO8Bs6FuKrCAvEIg6yNhEDIagUCYEOHgawyhnZb4oPoqjA2WoNo1X1MyfudM4lQMCjUcBDZO5UA7k/WugC3G11NrDJuF83GOLbKkvCYF1FRljPGJMKZ0xZlzAWe6riygE0auVJoX1CLETMVnbahNmFmKAnhFxNCMZcO4k0MwmU9QzVUdImoRAxh%2BJ4nvQuoszzmA0CaaoXRRkbNhnc%2B4NFcT82gCsyhM1PkIO4sBOC3gHBQW6ZovpOjBn6ILFhDQRiTFyjMXgCxVixnvVsWnJh0oXnrLCZcX41Vap02GLiKZnSfRBMoSEi5uLgE1iieU2J8Tb6JOSUo1JyC6JF3UFktGOSAh5PUAU00Wlim31KdEiBlSTnqBqbiXsoT6lysafjKU1jYhtOUQrF5yrbIao6YCxw9IFagt6dogZejhmYB1TSQmzd3p6u4pYAArLSWZJFWwLOddKf6B8BrWrdfMga1FgU0XYI8lu6hpxMp7KUh%2BWlkBmn8hE1AZgmCNEYIgNKLp7ZuFOSc3e8jaWgPpTE0VPYWV/KQek8NV9NCpV6CjKu1Lvjvi2mBaKsVTYTLkiPVgEBrAopabEFOe0pmYo2saLaOK0UMLTqjH1XMs7TvVuil83KwALtDtnCO8doZqtspsbYItfX/yPnna14zVWooPVsdAx7F0DUHU0q9GzmlJ3CpFCdEVJD3SlIoDCAgQCKCdYoYg1AgPWFA8gIDINpByAUECpw7BQN4Ag/%2BgD3YOD%2BkA4oTgoHwOKEg8QaDihQOSBANYYgqHCPoeIP3QgX4aAgE4EAA"></iframe>

But the real reason this method is bad is that it isn't as flexible as using `goto` for error handling. Here are a couple ways in which the do-while approach to error-handling and resource cleanup breaks down:

1. You can't have multiple exit points. With the `goto`, you can have more than one label if you have different ways of cleaning up. In the do-while example above, you'll see that a lack of multiple labels mean we still have to unlock the mutex before our `break` as there's no way to check if we'd locked the mutex without adding an extra variable.

2. You can't easily escape a loop within the scope of your do-while "loop". Instead, you'll end up having to add another variable and check to determine if you need to `break` a second time.

## Exception Handling

Ha-ha just kidding! 😅 While exception handling is actually available to C programmers on Windows (!), it's not accessible for cross-platform C software development. Call me a snob, but I believe that applications should be written to be as portable as possible.

## The `goto done`

Finally we get to the `goto done` method for error handling and resource cleanup. I don't wish to anger the ghost of Edsger Dijkstra so I should warn you that the key to using `goto` "right" are thus:

1. _Always_ go-forwards! **NEVER** go backwards!
2. Keep it simple! Multiple labels are cool and may help from time to time, like in our example. But 9 times out of 10 you only need one label. Also, I should have to say this but keep your functions at a reasonable size.  They shouldn't be 1-liners but if they get much larger than a page or two, consider refactoring!

Here I present to you the example code using the `goto` method:

<iframe frameborder="0" height="800px" src="https://godbolt.org/e?readOnly=true&hideEditorToolbars=true#z:OYLghAFBqRAWIDGB7AJgUwKKoJYBdkAnAGhxAgDNkA7PAZUQEMAbdEARgBZiArD45o2qgA%2BmMTEAzsgCuhRGwDkAegBUAajp4hqRoVTrmOAEaT1q5QFIADAEFLAJgDMOaomYyM6y04DCkvFRjZGRmADo4H0x1GPU1dQAxInVg0PMrO0cXNw8vH39A1zwIqNiY%2BKTCdQBbNHRCanUi9GB69TwATwAHdDMLG3tnV3dPdG8/ANQjYxKnaLKK5OqWZmREZQpCdDH%2BzKGc0fGCwldgWfnYxaqA/Rku9IGs4dyx/MmcZHOy8o1K9S6TrQKA9MnZ4k59ABaLp6TrqAAyJj6GUG2RGeT8XTwcC2jFQ50edk6PQwwJuMkQeG8AHYAEIDMqIOB6czqaiMaroHz0uxlABuyBwBg0um03Me1IAImyOehUCJRYwRHhxaDbPEAOroMCKZjMdQrNaMPBjbFjPSERgddTIYGK/6C2j1MyuJpuLac2hmW3qdjWawg1Gk1xjSW2AAqthEtgASjHbABNES0%2BEAeV8AGkRHQAJIALWifushNsAWNOEQ/2xuPl1RkJoAHsr1IqRBarSJVogANbjaUABXDAAkY5hbJKRABZACq4cwAA0RDmAHI58M52zw/OYGOq0vaPAVmWc%2BWtqmqDTABXGpXt60%2BaXL6fw%2BF7suHyuSHAAL3QzavrZ3iIX6/n26jFk4tJxBodA/mMPp3uoECuugrCengACUgbvkeIF/lSV7UDI1TXtoIioeg6FmA%2B4Hcj86jLsRxhtD6FHoW6BqEJa1q7PYYIXgyGgAALGCc6DArYqAGIwbLoAA7secotje6hsegtDtMg7RwGMwCrMYLCcdxYSCYJ6hCTClrVIp3zqJgaHqVS7KcmZFl6ByynaLZ/aOiaVQENpYxqRpiquVseByI0eCEDIYxlPZlGOeokgUgokiSBQMh6taeIYPiYXoBFDTqBQLCSHFMQJIwOCsAYAW5apDm0CZvK8ak%2BqMF0PTUKeN7kU1eAQCg1ABOoTIshoznoMQ6gCkKrKKlhlh0gysTtclB4yNRTjSqVzDlXuZRTb1ZHntQ8n9YlGk0U%2BL57qtMQ4MCEC3fCfY0VNNK%2BMtvgMc%2Bb0PjRi00jyti2fRtJ4pxwCSGAYCBuD6jAMgAWoDQXKQQ9NKSiWCwaLYepGiaBqyQp5KUpp/z1jajSyFUd5hAjsTnXJl3sTRyyE4gEB4baEDHaRSqYRhh2xE9SGve9O2k2zjlLStvLg/EqbUz6nK1IQHRw0ztnI6j6OizEy042qePqFqLY0DqVLIHJUU6YpM3SIYhU6mYqB3EYTDE/gjO8UdF3BXgkJRJ9NE3B7XT87KIuY4rj3PZLgPSyzsu0CHcxTfLoOI8rqvAurRBa/D/vg3rWlo%2BdhvY7jlwaFqOpbCk6CnEjOB8upNp286cA4PcPpmp5jBO1pcnarq%2Bo8FtVKMOlODAPbYxdL59R%2ByiAes0HGeYGH0uhXHfFg3XCJrL2g%2BIc7Y9jUIyWMBQqE5VJNpyKTjVXcUOtVji6B4iIdaNp2U%2BEBHAADZAJcQ7F2bssdQZYzUFjVktgurqQMHTIeb92IBUHigm0wJz4QI6C1I%2BsRS4Jwlv9KW0oAJ9TvNnBBx9bAEMtr0ag1tVINhwAEGarA8BuwNFzY0Yx8CBnoTEahZFEIc0NNzMMkZoxxkTMmNMmZszblZLzCg0cTyC2VKoDCMDRFNEThQ5OVCdG0JBoYs2VUapKXqgI4mhdNYlWSPaO82tSGIyRijLSMhqBQOrmUY2tddbmIIcBOCYFZFRljPGJMKZ0xZlzAWe68dsaNXKkYpChFiI6KDttHG0txG3nCXhOhaSzaMO4k0MwmU9QzWkYImoRAxhOJ4uvcGAszzmA0CaaoXRUnEO%2BH0%2B4NFcRc2gGE7iM1JngO4hE0CjgoLRPkXEpRiTVEFiwhoDRWi5Q6LwHogxFSxbGJfJQ9olEujlKGbnX41VaqU2GLiTJeFcHoPcSXDpXjvEBT8QEg%2BiNgknLEVMq0YERmBNiMUtspTIlLJois2JiiEkqOSZgauwLD5lBhXeSwABWWkOSSKtnyQS6UH1A4DWrsSvJA1qIOHpNLdggyzZ0FSr0WGXysbvi2hCmKGNYF2H%2BafEAcCNDTn8afQKRlwXOxRjpfy388RmCYI0RgiA0ouhtm4VpLS15YyxMq2s9Z0BNhFT2YBDgwE0PCVA45h9K5sBCVkygZVXg7XDptSQNzEbixehQuGlLN4DV9T811ksg0pype/beWdLEgq8ZsbY/MY3oTjTHKFtksXhpKlsdAqaQ3vwdUCqUWMsVY3CpFDaxotr3SlIoDCAgQCKHxYoYg1AW3WHbcgFtv1pByAUN4Zw7B214C7Y2pt3YOD%2BmbYoTg7bO2KG7cQXtih22SBANYYg47l2TuIB3QgX4aAgE4EAA%3D%3D%3D"></iframe>

Another advantage of the `goto` method is that with `goto`'s and a common scheme for the label (like `done`), you can wrap `malloc`, `strdup`, `realloc`, and other functions with macros to try to enforce consistent error handling when allocation functions fail.

Eg:

```c
#define DO_MALLOC(var, size) \
    do {                     \
        var = malloc(size);  \
        if (NULL == var) {   \
            goto done;       \
        }                    \
    } while (0)
```

`*`As a side note, you may have noticed my the use of a `do { ... } while (0);` there. As it turns out, this confusing-looking contsruct does unfortunately have a place in C programming as a mechanism for writing safe and reliable function-like macros. It is once again a bit of a hack, but it's the best option we have. For more information, have a gander at [this explanation on StackOverflow](https://stackoverflow.com/questions/1067226/c-multi-line-macro-do-while0-vs-scope-block).

So to make use of the macro wrappers in our original example, we have this final variant of our original program:

<iframe frameborder="0" height="800px" src="https://godbolt.org/e?readOnly=true&hideEditorToolbars=true#z:OYLghAFBqRAWIDGB7AJgUwKKoJYBdkAnAGhxAgDNkA7PAZUQEMAbdEARgBZiArD45o2qgA%2BmMTEAzsgCuhRGwDkAegBUAajp4hqRoVTrmOAEaT1q5QFIADAEFLAJgDMOaomYyM6y04DCkvFRjZGRmADo4H0x1GPU1dQAxInVg0PMrO0cXNw8vH39A1zwIqNiY%2BKTCdQBbNHRCanUi9GB69TwATwAHdDMLG3tnV3dPdG8/ANQjYxKnaLKK5OqWZmREZQpCdDH%2BzKGc0fGCwldgWfnYxaqA/Rku9IGs4dyx/MmcZHOy8o1K9S6TrQKA9MnZ4k59ABaLp6TrqAAyJj6GUG2RGeT8XTwcC2jFQ50eeycGAorjGABEAPIiACytnh8MpvggADc9MR1JIcAAvdAASm8AFZfAMyqhkN4AOwAIW%2BcvlsUswtF8rZVR85JqKzWEC5vL5PllQpFdgVOGBEAAcgBVBnjcka9RqgWWGWK5WmhWxYDIAjqcXUdCG%2BVKk22BWuzVe6OhlUxSPqADucBwrHUEGsBtBqJJZPUVJEACVMPTGcy1Ry9fyQx7w7FxVKjdHm99Y565SzkDgDBo8Ohql1gy33WGFX2B/b1LjmKtEKz2Zyefyh2263LzembXaNY7x10XW6I7WWz6/QGg04m83VxHJVHhy2b6q9JO90OH8a41LNcnU2MM1m9hErmgb5tSdAACqFuS1oAArziQKQyBQLrHvWEquleH6fu23xqpONyoHcEDGMhBqXvGaHrhaW7wvajrOo2lGjs2p4Sue74jl%2BZQJthzFfgmv5pgBhK2J0PQkpyeCEDIiB4I2X6IHAL4aNQjDVBe0pfp23bmP6jDaIajx3uoakaagIi6NoIh4EZ2bxAA6ugYCKDO6jakwfbtHAYx6IQjAdOoyDAlZjD/F2tD1GYrhNG4WwabQZjBeo7DWNYII5ugpKgeStgQbYIi2IWha2AAmiI0plgA0iIdAAJIAFrRKl1iiQEBk4Ig/zYriFnVDIfYAB42fp1l%2BQFIizgA1pOsEQQAEsWtjkrS1oQZgAAaIh1ZadUQXV9KNZghZ2bY7V4J1pnqegFmhSNqgaMAlkGYwIjjYFjq0ad52XVWI1PXd70iFWk6tRR8R0EuQXAu96YxegrAJXgAq7Gd2gXV1f3yU91AyNUz3WQj/boIloNDvElp48YbTJUTSOxe5hD%2BYFqMDGoqiihoAACxgnFl6i2KgBhhYGiZXeZo1hXTJPyX62JjMAqzGCwjPM2EnOc%2BoXMwv51Ti2M3yYIjMv65r2t6OpktyrBEV9lUcs%2Beo0u0JLZtbHgciNNJMgG7ERvEy7kiyQokiSBQMgzoFeIYPibvoB7DTqBQLCSL7iSMH%2BBh%2BtHTvG7Q6umqjqTMO5XQ9NQt0vSIzt4BAKDUAE6hKSp%2BscjpPaSweWm4cXUkGTIZiOsnzCp6dZRmTdBOvfJqii9XefyZ9trwqdX4blay/0U4moT1KIqSr46i0VvmqhV33GXBo0p4ozwCSGAYAZQqbH%2BjQmkCXeokLBotgzmsBm%2BVMugMWNxZKywlF0AaQVGiyCqO9MIT9YgFjpAyJkEA5410rEuYK6DrqV2ssjci3cgJrjiBoJyr9qAuXksgRMXtHYT0rBKVgeAXJmCIl0Iwnkxj4AQajMoBZILQTgug4B88A54EhFERh%2BsiFf0vuoJyLktgpHQKcdQwAcAshJkFOhUUUz3GSvLSWTCkzOVciXHgA95KMFDjgYA9CxhdFtvUPhKJx5iJrlIuYu9HShVXrheI8I1gzWMbDaQZim5CE5IwCgCMo5CyCnIIBYsa5uK/FiHE6A8QiH6kNSaISICOAAGyAyZhNaacjszf24j/MuJMDCwKtjXdoEpjENOht5Xy5SOgF1IYgmI69j47m3hoqeb0enn1wt/AWPTX69CoYoeS6BBo4ACByFhbD3J/24U0GeKIL4xGQaWNBAMq7vQ5LlfKhViplQqtVWqR09JVhwRPfB09VB8iqf0704ygYg0dFcgqRUSrlUqkyGq9UmoBNIQmBGqcmgWhxnjcZNdB7b0dGcsaPTgZLimT8n4szmZNDMOHGcHIPIAJqEQMYGlaiEBZu4%2BUBYlqoOZFi16Fz0wcomczXFvJvAOFlECm5oL7kQseU1FGi5eSvLweMvAnzvnPz%2BTigFQrAV5WBbcsFDyoWYDHoqT%2B1TfllOZkqaUyL8Z3TRUqTUjoMEL0NTEK1qKF6Dw1aM9gMKZl0GDr0e%2Bj9%2BGxHOgPV8Ml37ZhkNQaaIAvzxGtDGkJXTVYBU5BKX0Pl7ZZLxGYJgjRGCIBDtFGhbhaU0vSbhTJvVckDRWSIaN00ikOFKecnFlSYXnjjSawZFph4IpGZqUNkh8VmhopvB%2BDrPEL1Hc2IZE6wBTsTOIpG3jMAT1nQ%2BTY2xRHLq8dI66yrrzGoJd8bd6Bd0rplkeuUkYP4Oh7VOeOns%2B4e0kKvE9AwihalcKyLsqBN2IvTN7V4GLRmMHqRXN1Ejm0OHmgjVYjgHAcj/bpT5SGHJEGYKgJDXyFLTNiACIoFBYOCxjrnCRrTU0dFDNQXDzrvjuxfWDYhPET1lCI0CWDCQM6sCzhKHOLTs49No/Ry8X4mOJ29eJzIJ7FB8gECARQgpFDEGoEp6wqnkBKcPtIOQChBVOHYKpvAGn5MKamhwNKinFCcFU%2BpxQmniDacUKpyQIBrDEFM458zxBtGEC5DQEAnAgA%3D%3D%3D"></iframe>

To end this piece, I'll leave you with some supporting links for how to safely use goto for error handling, and why perhaps do/while(0) can cause problems (with loops):
- https://embeddedgurus.com/barr-code/2018/06/cs-goto-keyword-should-we-use-it-or-lose-it/
- https://ayende.com/blog/183521-C/error-handling-via-goto-in-c
- https://www.cprogramming.com/tutorial/goto.html
- https://embeddedgurus.com/stack-overflow/2010/02/goto-heresy/


Until next time!

-Micah

## *12/11/2020* - Postscript; The `else if`

There is one additional error handling pattern that can work quite well in many situations. I call this one the `else if`.

This pattern works when you're able to construct each line of code as a series of `else if` statements that test for failure with one final `else` for the success-condition. Resource cleanup happens _after_ the final `else` block such that each failure condition and the success condition a followed immediately by resource cleanup.

However, when following this pattern you may at some point need to set a variable in the middle or make some call doesn't chain. This breaks up the `else if` flow and can complicate your error handling pattern. It may be an indication that you should break up the function into multiple functions but resolving these `else if` interruptions in an elegant way often requires careful planning.

Here's an attempt to apply the `else if` error handling pattern to our example program:

<iframe frameborder="0" height="800px" src="https://godbolt.org/e?readOnly=true&hideEditorToolbars=true#z:OYLghAFBqRAWIDGB7AJgUwKKoJYBdkAnAGhxAgDNkA7PAZUQEMAbdEARgBZiArD45o2qgA%2BmMTEAzulaI8OGuXTVUAYWTMArgFtqIAEwAGYstQAZHNXQA5HQCN0hfgAdkk/Aurqtug8dfu8jQWVrbaDk7sUjLocp50eIyEeN46ekbRskHUCUl4ITb2jvySicmpvhmleQVhERwAlFLImoSIbADkAPQAVADUuSpJqH3MOHaSfT1dAKSGAIIz%2BgDMlohaGH0zy6qlqHbIGgB0cNuYfRd9vX0AYkR9BxpTswtLq9Trmpvbu3i4tCczpcLtc7oQ%2Bto0I5qH1LHh0MBHH08ABPZzoSbTOaLFZrDboLY7PZjOyA5bnYGg%2B7aFjMZCILoUQjoAlY164j74wm/QiWYBkimXKng0qEVCaZzPbFvPFfAk/PYKAXAkH9MF9Zy82gUKWvBbXZZigC0zjyKL6Fgmupx70%2B3x2zjwcGZjFQAulC1R6IwOtFmjkWwA7AAhbHAxBwJJTPrURjadDbUMLYEAN2QOBG/VQjESielgYAIjG4%2BhUCJs4kRHg83r5tcAOroMAdZjMPq0%2Bk5glOglJQiMc3IHUVxga9O0RyTSywj7M%2BO0SZDvrsQyGa1vH2WAkF%2BYAFXmInmACUj/MAJoiYNmADyqgA0iI6ABJABa5xXhg982q8kQGqdLpltomjwgAHlWfQjiIfYDiIdKIAA1oSRYAAq7gAEkemDzAWIgALIAKq7pgAAaIhPtYT67k%2B8xmK%2BmBHjW36JL%2BxbxmWUF4FM/TAOWOaMNBhD9ua2xFtYBFmGYTE/jgf7uAAXugEG8VBMEoiICnyssRafsswaUv0dA4IpfRLmpfQQNOMTzngDTWjJcnGUpXG8dQOh8ZW1nKHgkyiX0un6Vc/R1EiS5ebQM7tkJA7rgsjxth2TDwiIwB0nYLAeQJakiDgFAiFYpalhAdkzCGYaXLG7GZRBPT9PC2jOEx5UXLlFkQOJknIX5Kn8YJwklfoABsSzDcmKoXO1ElmF12kWT1lbmX5NKtvSEA7vuh4nuel43vej70dGmlDhAlWltVeA9A0V1XUGSbzONqq3IwOCsCMBDtitSUEvGkKEOaVDgiOUXCWAYDWg9fTMngrQwhQLDSExwKlUWMjSLdzUqvNWXRepmnIX060Hsep4Xlet4Ps%2Bb6I5cyNfsCrVQK57lQeFPkzUWWN9bBmkDcNQ0Y8Ck2daJfkQPVkp%2BS6n3QNVanEHNss4xpTlbPo%2BmE5tJM7eT%2B1vnZ/RHRQJ0lhxvUXddN2lXdEPXPMON9AA7owU6TBQmitvLQgjA7BJwy9pbIsgH3wV2EJEN96C/eabL3Q9UMw30cPMAjekY8jfSo/KZVjeNnOLbN4vU7nivCcrJlLKGs0a8T21k3tlOYEXQYFnTlzx4QMJ4IQmgJqnryFl%2B8XtqgZas8bVWcVMVgOyIrMldnscXEPP6aL5s1Jynd0Y9cZj0khPbAzFkhBz7fRMDCkiMBQMjmq6IwtOC08Z6wNlHODlyOs66CuiIwFgXBe8IAjVUkreCCEGhNRzkFAW/R5jOHRCoUyrRIL8WfpHbygdkRwAJKYUyOoD5qSODAl4i9YQ6koPDLSLdZqJS7ClNKGUQGl1yvlFkGBUDFXntbB6oJnqvUwbQ%2BEWCCSpWQOlNsQNzIxyRoWZ%2BaMrYCyFD0RRlxDL%2BnaJITRKiLjSIennHGMwACswYmbaGqqzSQRiix%2BWnrPF%2B3km7AlMeY%2BxC5VaVyLOwSBpDgQrzXkWLuPcm601rAZPoBFqBgOEYfc0x9TI9nBABb%2BqBJjn3bIgDRU4uI0HaGHZkb9dEXE/oBX%2BIF0DgU0JEwBwDerZTARAvuiwoHtwvixVeTUB61l6Mo5M/QAACdheToB1PMEe7YYzoAdmxAOQNWaYIPqI8RMSiG9LDP000/ZtDTPGpgVxXFTprL6H0jZcYUGJHGihcc8JElBwPnMkchy%2BktORN3AkwJdnoIipIdRGJJBu1bLfEepYVn3X6c8jebyLg3D4QHd6d80GvzWSQoejB4GmBcZ8vAEAUDUFKGfSM4J%2BinXlmmDM0YRxcIxsvNp/jE6UKbqdU2lYuI9FsXMvyHUpKNIxgzTl7NtmlVUIKvofKRazQpejKBYTgyuiisASQoN37jU1HCI2Sx9B33yibEQzI5BCFSmwPoT5qAphYGSpIwAdDeQVWAIxqhqDqoadwi46dM5kIsqK7Sos2V7PxsteCEBDbjzOpxC2kqfFKL6NeECeCISRyICiRVhTm5yIJLyqa/KTqTLsZio0ZxTr41FOKZwwbroKKlZG6N2SdQ/QTUmkhMiUbJyzs6lUDYmwdGZA8dAfI%2BjABwCmZQpkHZWEIJIOAOBJRLgPiOeWcSfbNlbH0Hgq8uLO3cMATu2CxxwkcAUhtD0fW5vzSWfGDzuUVpauQiF/LNVjyPTZS2C8IZhOhf7N6Qd4VzOTY21N4aX1KO0SqNRGTfmWMveNH9D0VXaiAfoDVI8tVVV1YkYQrAQB9FGewvozYliGIVR0BFGD3qSJxqDO1Dr4Py1Ok67RITSH0e3v0VQrAhB9AlJ7SYBVsPJoZjesVRY/GUog%2Bmzq5HZoPu8sJiN41RPTXE0WSTtA83kho/%2BgDKomQsizTPVmKnMA0ccQ9ejGnE7MnQDpnNj6jMus6QxuzGNnl%2BI6S3BYHQmjMBAB0QxHRiB6A6IYXzyAvOqD6MfVouS3jsF83gLzgWrrEAQhwVcAgvOcF8/5wLxBgsdF85IEAxhYsBfc00QdY7PAgE4EAA%3D"></iframe>

The `else if` pattern is works out ok for a few calls in but it breaks down a bit when you need to set variables or make standalone calls like our call to lock & unlock the mutex, or to allocate memory for the global data array if needed.

With some careful planning and well structured functions, this pattern can be quite effective, but it does take a surprising amount of effort.

I used to like the `else if` pattern quite a bit. Nowadays, the `goto done;` pattern is my error handling method of choice. The `goto` pattern is easier to read, write, and maintain.
