---
layout: default.liquid

title: Error Handling in C
is_draft: true
---

# Error Handling in C

It is far too easy for C code to spring memory leaks due to shoddy error handling. In a larger code base poor error handling can result in years of agony tracking down bugs and digging out from small mountain of technical debt.

No thanks to Edsger Dijkstra's infamous 1968 letter "Go To Statement Considered Harmful", just about every C programmer has been told at one time that `goto` is "bad". This is still true but for one use of `goto`: error handling.

In this article I'll present an overview of a few different error handling methods available to the C programmer. To illustrate, I have a somewhat contrived example in which a multi-threaded application stores named data elements in a growable global array protected by a mutex. The example function adds one element to the array, and allocates more space in the array as needed.

By the end of this article, I hope you'll agree with me on the best option for error handling in C (_hint_: it's `goto`).

## Multiple `return`'s

Perhaps the most common method for error handling, especially in older C code involves the use of multiple `return` statements. Allocated memory, thread locks, and other resources are cleaned up as-needed at each `return`.

Without further ado, here's our first variant of the example program featuring the "Multiple `return`s" error handling method:

<iframe width="900px" height="800px" src="https://godbolt.org/e#z:OYLghAFBqRAWIDGB7AJgUwKKoJYBdkAnAGhxAgDNkA7PAZUQEMAbdEARgBZiArD45o2qgA%2BmMTEAzulaI8OGuXTVUAYWTMArgFtq/ZagAyOaugByOgEbpCHAAx3iAB2ST8C6uq27%2BLt/JpjUwtta1t2BykZdDkPOjxGQjwvHT12KNkA6njEvCDzKxt7R0kEpJSfdNLc/JCw4oBKKWRNQkQ2AHIAegAqAGoclUTUPuYcS0k%2Bnq6AUjsAQRmAJgBmE0QtDD6ZldVS1EtkDQA6OB3MPsu%2B3r6AMSI%2Bw40p2YXlteoNzS2dvbxcWinc5XS43e6EPraNA2ah9Ex4dDAGx9PAATyc6Em0zmi1W6026G2u32Y0sQJWFxBYIe2hYzGQiC6FEI6EJ2LeeM%2BBKJf0IJmA5MpV2pENKhFQmicLxx73x30Jv32CkFINB/XBfScfNoFGlbwWNxW4oAtE5cqi%2BsYJnrcR8vj9dk48HAWYxUIKZQs0RiMLqxZo5NsAOwAIRxIMQcESUz61EY2nQOzDCxBADdkDgRv1UIwEkmZUGACKx%2BPoVAiHMJER4fP6%2BY3ADq6DAHWYzD6dIZucJzsJiUIjAtyF1lcYmoztBskxMcM%2BLITtEmw76ETsNvevpMhML8wAKvMRPMAEpH%2BYATREIcMAHlVABpER0ACSAC0LqvPfNqvJEJrna7y20TQEQAD2rPpRxEftBxEelEAAayJYsAAVdwACSPTB5kLEQAFkAFVd0wAANEQnzMJ9dyfeZDFfTAj1rL8Eh/EsE3LSC8CmfpgArXNGCgwgBwtHZizMfDDEMRjvxwX83AAL3QcCeMg6DUREeSFRWYs7CTa5%2BjoHAFL6ZdVL6CAZ2iBc8AaG1pNkwzFM4njqB0Xiq0s5Q8EmES%2Bh0lYQzVPo6mRZcPNoWcO0Ewd1wNHoenDfoAAFLD5dBdXmVARjHUwAHdWLLCC%2BL6MLOIIFE4EJYB6UsFhIqE44EoSvpErNAdtHy1U%2BkwVgrPypqWsSeNCoSTrkInBEITK3tip6zzhsYfqWTwVpYTwQhNEJEFuvQXrJADdpJEkChNDbC03Qwd1FvQZbCFhCgWGkEFbkYHBWBGMrzpmnbPIalN2XmJ520YJwMRUNz%2BJKiAUGoUo%2BkjaN%2BjjBNiD6dNMxjUcbJmUNwyuJGy3B8CelykQSqQoLxMk/zPxBHBdQgMSJKQnz8eDVRsdUCmmZEnzMeDZN5k6y4tXhCgIGWJZzpEfHyxZOQhCqtg%2BifahUxYdHEmAHRPMkMAwBmABWVRqAlhpGKFvolpWvp7uYaRzaubHCxp4V%2BnmNsuwRDtY3QPL/UDMqnGA4zYRaCExlKY4bRBEmyZ82kPcQCANOHCAZcJ6yzeplMrjpszGcMZmtJ9nLSdm2hjXOfGsZxnPOpua9g%2BXBMoUIVE9ejoWRZ1cWlklzLpdLWWYgSYRWBAO4XrelFkA7RPu0hHaiAtKgIVy%2BavqsvXDeN02Hc6q3bpth7E2zwXHaLF3AqbCCaBbTjkBy1aKvylHJFn1g8BbSYJScMYmC9vgKO/0Y6%2BzLt9CuVdSzkzFL/NOpYs4C1pvTAuRdiyx3LngSuFJq781xvXfojcH66hbsvDuIChbMlZGnMBJVEH4NVN3PAYsJZS3TnLUeisJ7PVegVD688vYoCcEOEcRV8bbyNibPu9C66qkPndE%2B%2B9gzOzrFSfoTYWwskeOgfkfRgA4FTMoYyT8pxwBwFKZc01Rxv1njlZsrZ2w8E0LDRgh0cDAGfoSFw8IbDANeOfS4GCIFYKgQmcmo5GIMJuIYBkiFpoR04u/Podi4ZCD6JIRgFAZBnUysZVoJdN4/U7sLf86A3QiCAqBWCsTe4ADYVJRTUnBeCMjFiyN6AwmM8wQYGDyRCUchTwpTRfr0yxL9VK/QCZcChlw84M0pmgvRhNVI1yQULZSfEBJCXJgnOCEAdz7kPCec8l4bz3kfHRGMKcxbpw4j0BorSLZzNQTzYuGyqwrLwbIi2TCWF9zYUPEQHCFbj0nrw96s9OwAMJKQtuNsHgDNUhI3e0jInfMoSydANDS4lWwZgauSiLZUKxUEqyjyLZXHkcfO2p81mdSdp0q47z%2BKqXUg5cmByDzHlPBeK8t4HzPjfEop2M1HpzOcq5SCJVvJaR8syrZMENKrMZYFeYjS4STGOm2FGUKF6txhUvOFMzVS3M2ZxOKKIdpODRVMkECJtBSh8q6RO0BlmNJRq6hpQk2VGWWAFTlRyeWnP5Rct8Nl%2BjXPgWxDO9zyWdWeQs15xZ7VOGVeizqvze793LOwkeIKlY8OnmVdYrpHoaWMqIkaSL9aSL3mfFVqpiXYvAVZPFBKz4UquE20lnk40Uqpbbe2HahYMvTUsr10UfIpsJUyt13ry1%2Bp8gG7lJy%2BXnMFZgYVl9VGzonaiQ2IYJXaEJtKw2xYWa0MwUoo9J7MHeSWGGYu7AbVqL6PhagzTyqEgSek2eyBeyTRdGU1AkwmCwkYIgA604H6fBhUQdAfiGFOiA%2BUyp6AwKaA/TU5Y9TNmsuaa0hhVK1obUidu%2BYHQmjMBAB0A2HRiB6A6HYejyAaOc3fq0do2xVjsHo3gGjzGHnEHgsUAQNHOD0cY8x4grGOj0ckCARw/GmOUaaIYwgbhFCcCAA%3D%3D"></iframe>

As you review the above example, note how much duplicate code exists for resource cleanup during error handling. Can you see how easy it would be to forget to clean up a malloced pointer or a locked mutex. This is a recipe for disaster!

## `do { ... } while(0);`

Another surprisingly popular option for error handling in C is to wrap all function code in a big ol' `do { ... } while(0);` "loop".

I'm not a big fan of the `do { ... } while(0);` approach to error handling for a few reasons...

First and foremost, it's an obvious hack to perform a `goto` without using the word `goto`!

Secondly, it's convoluted. To the novice programmer, this approach for error handling is non-obvious because it abuses the do-while looping construct to perform another task. An experienced programmer may appreciate such cleverness but the simple fact is that code which is hard to read is hard to maintain and increases the barrier to entry for novice programmers with little if any real benefit.

Additionally, I think the extra indentation of the  `do { ... } while(0);` is harder to read.

Here's our example using the do-while method:

<iframe width="900px" height="800px" src="https://godbolt.org/e#z:OYLghAFBqRAWIDGB7AJgUwKKoJYBdkAnAGhxAgDNkA7PAZUQEMAbdEARgBZiArD45o2qgA%2BmMTEAzulaI8OGuXTVUAYWTMArgFtqIAEzFlqADI5q6AHI6ARukId27YgAdkk/Aurqtug6/dPGjMLa207BydnaVl5Gjo8RkI8Hx09Qxj0OS8EpLwQq1t7R2jE5NS/DLL880Lw4qiASilkTUJENgByAHoAKgBqXJUk1H7mHBtJft7ugFIABgBBWf0AZnNELQx%2B2dXVSTxUG2QNADo4Xcx%2B6/6%2B/oAxIn7jjWm5pZX16k3Nbd39w7mPDnS43a53R6EfraND2aj9IHoYD2fp4ACeLnQUxmC2Waw2W3QOz2B1Q4xsINWVzBEKe2hYzGQiG6FEI6CJOI%2B%2BO%2BhOJAMI5mAlOpN1pUIOhFQmhcb1xnwJvyJ/1JCmFYPBA0h/RcAtoFFlHyWd1WkoAtC48mj%2BmZJga8V8fn89i48HA2YxUMK5Ut0ZiMPqJZo5DsAOwAIVxYMQcCS0361EY2nQuwjSzBADdkDhRgNUIxEim5SGACLxxPoVAiPOJER4QuGxZ3ADq6DAnWYzH6DKZ%2BaJrqJSUIjCtyH11cY2qztHsU3MCO%2BbKTtCmo/67Hm8ztn39tX6xcWABVFiJFgAlU%2BLACaIjDJgA8qoANIiOgASQAWld1/NvYsDvmcEQbVXXdSttE0PB0AAD1rfpxxEQdhxERlEAAa2JUsAAUDwACVPTBFmLEQAFkAFUD0wAANERX0sV8D1fRYTA/TBT3rP9EnkICEyTSt4LwaYBmAKt80YBDCCHK1dlLSxSJMEx2P/Lj%2Bg8AAvdBYOE%2BDELREQ1KVVZSx/VYwxpAY6BwdT%2BlXHT%2BggOcZHQJc8EaO0lMAlTLI0gThOoHQRJrRznKmaT%2BmM0zbgGMIIms/UguUAS51szlliNXpekjAYAAEbAFdB9UWVBRgnCwAHcy14uDRP6eLaFRZBUTgIlgEZGwWC7CTh1OTLMv6LKLSHbQKqJdVMFYZzht6/qkkTKrEnVfpMKnSCoQIRqiVqgTxymtk8DaeE8EITQRpuManISlSgw6SRJAoTQOytD0ME9Hb0D2wh4QoFhpDBe5GBwVhRjWp6avGhLurTFKXk7RgXExFQArEzaIBQagDn6aNYwGHj0GIfpM2zONx1c2Zw0jG5oZUzjNBCwz%2Bi%2B5hpHYsEcb40TYN6MqRE2jD%2Blk%2BT2PJ65UAa0nU0WBbrhwfUIH5kwMNCnHQ1UUnVD5uT5ek0LidDcXJfVO4ww9DrgEkMAwDtfWwVy9BGFQ5nJdJ4tf31u5Fg7HtIK7eN0HKwNgzWlwIOs%2BFWihHTTktyWuZ50L6Q9xAIH00cIFZxHa0aRoHYW6W7LlhW6ZjsHaBJsm0ytjV%2BjvYPVyTGFCDRc2o4rm27ez9UnZdyXmyJEXqDbATkFKg6muGvHJAa1g8DbKYpRccYmC9/BI5S/Wi/O2hTUuJXQolOfU/LLOTKF9Vc9ljWC9LdfnK3qkcdLvWK8iqua/1OuiEbi3V6f1v7eP8uFqdwbK7AYLY2xsmeOgQU/RgA4HTMoayw8ZxwBwDKVc/Y5qMHHg1UqrZ2ydh4DTASjAbo4GACPIkbhESEBXu8CW0dfbc2LngW%2BmAd5022v/VK9CFp3BMEydCGDbIT36LgjGQgVKMAoDIR6RVrJtB9uVTatCT5ghdG6W2YEILQWQgIiAKwABs2lOq6RQqhI%2B4tVGigygAhaAxFhw2MPIqE45QYbwEmtDBTj0GjwjlYm438c4y3zlrOmWl2Y6Qfv4g29iTFwWQFifunQBLQRwAcPGU8Z5dgTr2BEAkUrRLBOEmstk47dkTvuI8J5zxXhvPeJ8L4WJxmThQA%2BvF054F6Jndu%2Bsz4hMMqFYpYlIm60Kd3TU/1Ab1WyShXJ78G70yeK4nSTdAlP2uOo0CIhwKQRgpoagZj9H6CMREkxui0IWMrubJuLYqYA07DGeB0z9lmPWtCbRUFIFUAgWI3%2Bzd1nPHdH/R%2BFcgHcPWUM8Skk9JeV5pU48Z4LzXlvA%2BZ8b5PyC1sWCJ2oMfpn18v5eCm1abOzCenHSML1JRKxeM/oiw4lpPpvdZgeNylzKIESeZVo1lr3LGzGs%2BSBiQW0C4TFPCrbCplKFd0CdoDkpMXjOVxjoX6R2PoUy8LqlIrqaixpn5XIDBaW0isHSukWLGVLYJF9QmlkldS8V6yISTIrNMjY7ofqqtXMskxqy6EApuJszR2yPkiBeXowxyqkJmMuc/a5FtbkHHuf0R5fYGphrQm8nZ0EvkctEUSP5PKf5Ap6Y7EsFqYHyskrzSVJaFqQopaqlYEY6aasRbUlFDT0WYFraGZ2wDJb1pMbMAArGGAl2h07EpHaWRWjDNo9vHZO5hIV1WhXYGKkB/RSIHIEW84RDVkD9lWhoj0UwmDwkYIga6s5B7fE5RylRNKNkgSDVmvZO60JHJOSUs50aN20roFdLEZsv5%2BslkpGm1ajrJi4diksoiUGsAgPMc1/bLV2QZj9G1VN8w03tb0q18l%2Bjm1naVJh7j8NPz6RfEjhc53MNYffUZT6rasnZKnej7jGOHx7XBvtDrJZsfQBxsjm1UMCeuGCvjXdri7X2jhvakhBYlk6M0ZgIBOjDs6MQPQnR5jaeQBptWE82gdDVasdg2m8Aaf05nYgqEOAbgEBpzg2ndP6eIIZzo2nJAgHmMQazenVPNHgYQDwihOBAA%3D%3D"></iframe>

But the real reason this method is bad is that it isn't as flexible as using `goto` for error handling. Here are a couple ways in which the do-while approach to error-handling and resource cleanup breaks down:

1. You can't have multiple exit points. With the `goto`, you can have more than one label if you have different ways of cleaning up. In the do-while example above, you'll see that a lack of multiple labels mean we still have to unlock the mutex before our `break` as there's no way to check if we'd locked the mutex without adding an extra variable.

2. You can't easily escape a loop within the scope of your do-while "loop". Instead, you'll end up having to add another variable and check to determine if you need to `break` a second time.

## Exception Handling

Ha-ha just kidding! 😅 While exception handling is actually available to C programmers on Windows (!), it's not accessible for cross-platform C software development. Call me a snob, but I believe that applications should be written to be as portable as possible.

## `goto done;`

Finally we get to the `goto` method for error handling and resource cleanup. I don't wish to anger the ghost of Edsger Dijkstra so I should warn you that the key to using `goto` "right" are thus:

1. _Always_ go-forwards! **NEVER** go backwards!
2. Keep it simple! Multiple labels are cool and may help from time to time, like in our example. But 9 times out of 10 you only need one label. Also, I should have to say this but keep your functions at a reasonable size.  They shouldn't be 1-liners but if they get much larger than a page or two, consider refactoring!

Here I present to you the example code using the `goto` method:

<iframe width="900px" height="800px" src="https://godbolt.org/e#z:OYLghAFBqRAWIDGB7AJgUwKKoJYBdkAnAGhxAgDNkA7PAZUQEMAbdEARgBZiArD45o2qgA%2BmMTEAzulaI8OGuXTVUAYWTMArgFtqIAEzFlqADI5q6AHI6ARukId2ABmIAHZJPwLq6rboNuHl40ZhbW2nYO7M5SMuhy3nR4jIR4vjp6htKy8jRJKXihVrb2ji6Syanp/lmVhebFEaXRTgCUUsiahIhsAOQA9ABUANT5Kimow8w4NpLDg/0ApE4Agov6AMzmiFoYw4sbqhWoNsgaAHRwB5jDt8NDwwBiRMOnGvNLq%2Btb1DuaewcjnhcLRLtc7rcHs9CMNtGh7NRhuY8OhgPZhngAJ6udBzBbLNabba7dD7Q7HaY2MEbG4QqEvbQsZjIRD9CiEdCk/FfIm/ElkoGEczAam0u70mEVQioTSuD4E77E/6kwHHBSiiGQkbQ4auIW0Cjyr6rB4baUAWlcBUxwzMsyNhJ%2BfwBh1ceDgHMYqFFCtWWJxGENUs0cn2AHYAEIEiGIOApebDaiMbToA5R1YQgBuyBwkxGqEYyTTCrDABFE8n0KgRAXkiI8MXjSsHgB1dBgXrMZjDJkswuk92klKERg25CG2uMXU52j2ObmJG/Dkp2hzcfDZxOB3fQMNYallYAFRWIhWACUzysAJoiCMmADyqgA0iI6ABJABaN03vpWFULOCILq7qetW2iaCiAAe9bDJOIjDqOIjMogADWZLlgACoeAASZ6YCspYiAAsgAqoemAABoiG%2Blhvoeb4rCYn6YGejZ/sk8hAUmKbVnBeDzCMwA1oWjDwYQI42gc5aWCRJgmGx/6ccMngAF7oDBQlwQhmIiKpKobOWThpvcIx0DganDOu2nDBAC5xCueCtA6imAcp5nqfxQnUDowl1vZyh4HMUnDEZGwRlqwzhJElmGv5tCLj24mjtuJqDIM0YjAAAjYQroIaKyoJMU4WAA7hWPGwSJwxxfxBAYnApLAMyNgsIlEnnBlGXDJlVojto5WkpqmCsA5A1dT1KTJpVySasMGEziiMJ1YO1UjQF02MONHJ4F0iJ4IQmiDXcw3oKNkghj0kiSBQmhdjaXoYN6W3oDthCIhQLDSBCjyMDgrCTHVD2radAUdRm3IrG83aMK4OIqL5ok1RAKDUBUwyxvGIzcegxDDNmuYJpOTmLJG0Z3FDykcZoQUGcMH3MNIbEQtjvEiTBgylSINXoZFsnyWFv4QjghoQDJcnocF2PhqoJOqLz4tScFRPhumKyzRFEZeolwCSGAYAOurwzAMgdWoDQqYCxmdwk6WgviiMKxdn2KI9om6BlcGoZ1a4EGWYinQwtp5wG3cnPc8FjJO4gEB6eOEAswj9atK0TN3MLNliyYEu02Ha20MTpNW7NDz3r764pnChCYnrIezcbpvm6ntw23bEVtrBNAdvxyAlXtDUDbjkjIFML0dnMMquNMTAu/gwcQ8z7tc3neDmtcUvBVKE/x5WKeW2raci5n2flrnIO0KvNLYwXquGyXZeGhXRDV/r8/q/Xw9mxYTfhrbTZ0iMbYOwcleOgYURscCZmUJZXuc44A4DlOuFak5B7DxKu2Ts3YeDU34owK6OBgB91JO4ZE9g56fH3rcU%2BDkL6YHXrTScbEyYRRMCyNCK1rJD2GGg9GQhlKMAoDIe6hVLJdDdmVGqZCmHAQ9OgL0IhwJQSQqwiA6wABsWkko6WQihXeqspFDCkQmFYsNjAiJhJOYGo1lr91MYg/uQdDGv1uOnUWfNj5G0Ttpa%2Bhj7bDBWJojuuJqBd2qpBHAFRcasDwGPHsUd%2BxIn4hDHxtxNJs2shHXs0cDzHlPBea8t4HzPlfMxBMscKDbx4onPAgxk7fyFofNxitaapLrF4lWyTNRQl%2Bv9DEw9MnxMflXOmLwLHaRrk4w2RsTbD00NQbRdTrZllbpqFpoltK6XcjzbJJ5zyXhvHeR8L53xfkYUXH%2Bq0vouK8j5OCNUaa22aZ4zRGy1LeLOf/PxATwl01uswXG/SXaV1JIMm0EzmaVlZnWRJIwUTaFcKcihmpYVymCp6KO0AnkSVxhijREkXkqn0OFbZuS9kFMOcUr8TkRhlIqVWKpNTdEdJcUfJp5ZkVvMRbfbU3Sqy9MXIgT0X09IxQ2m1Uc4zyGTLuO/YYsz5l70Ni3d50rMXJWCsihZKTVU6WFesKMtNiW7PyQcopxzMDfyVWsM5qyxISUWAAVgjNc7Qic7kOvLJLReNVv7OtdcvIKhLgrsARR8ugF1cS6xfpKu4ilqY832odBFcrWEgH0SMEiczWH1SHAEzhJsGpLRkV6OYTBESMEQJdec3dfjAqIOgSRZy3RFrAhBdA0Fk2oRUfodRaTnnaMZU2T%2BbBlkuPpl9VllNCzUw5YbZlbi9aepKkvM%2Bjl2nKtmnO8WC6c5euXjQq%2Ba7OVSrphydA8dd0rv3TvTVmpLXHpPZyc9S6aoDqPc3JZZzLVSO2rtSdO1JCMLLL0dozAQC9Htb0YgehehOEg8gMDcsh5dB6PsTY7BIN4DA7B5OxAUIcCcC4UDvROCQeg7B4g8HeiQckCAFwmGYPAfaJAwgnhFCcCAA"></iframe>

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

<iframe width="900px" height="800px" src="https://godbolt.org/e#z:OYLghAFBqRAWIDGB7AJgUwKKoJYBdkAnAGhxAgDNkA7PAZUQEMAbdEARgBZiArD45o2qgA%2BmMTEAzulaI8OGuXTVUAYWTMArgFtq/ZagAyOaugByOgEbpCHAEwA2YgAdkk/Aurqtu/q/fyNMamFtrWtuyOUjLocp50eIyEeN46euzRsoHUCUl4weZWNvZOkonJqb4ZZXkFoeElAJRSyJqEiGwA5AD0AFQA1LkqSaj9zDiWkv293QCkAAwAgrN2AMwmiFoY/bOrqmWolsgaAHRwu5j9V/19/QBiRP1HGtNzSyvr1Jua27v7eLhaGcLtcrrcHoR%2Bto0DZqP0THh0MAbP08ABPZzoKYzBbLNYbLboHZ7A7jSzA1aXUHgx7aFjMZCIboUQjoIk4974r6E4n/QgmYAUqnXGmQsqEVCaZyvXEfAk/Il/A4KIWgsEDCH9Zz82gUGXvJa3VYSgC0zjyaP6xkm%2Brxn2%2Bvz2zjwcFZjFQQtlnNWGAoJiJABEAPIiACyi0MhiDqggADcksR%2Bu4AF7oRo7ACsqlxoNQyB2AHYAEJq0tl66zLM5svxyG7ANQ%2BmMiAptO7EuZ7NLcs4PUQMwAVUjxID9f6tfTs2LFar3fL12AyAI/TzpnbZcrXcW5anDfn%2B831auu/6AHc4DhWP0IPNGl67b7/f1gyIAEqYCNRmO1xOt9Olw85yuPNCw7fdwLVQDtxrZAcFGAZEW0Zx1wgmct3LRDpTHN1mAZRA4wTJMcFTO9Vg7KCez7Qdh3rMdMMnacd1naD90XZdV3QFCIIojcCz3VDuOY8taxHVF0CQriBJ448%2BLPC8rxvO8DQfdA/VMZ8QzoAAVV8AwHAAFAiSCeTQKEnITrhAqcwIEtCjzVESx3FSVnAgSxTNI8iLNLXtr2owwRzHCdQOPbz5zY/MOMk0L0N4/jbM7eyT3PS8iUU%2B90UxX0kzwQhNDkUD7MQOAkmmfpqEYbROLI%2BzY1g%2BCV0YRJ21lWSKqq1ARFQJrGBEPAWuU24AHV0DATpcP6JsmERVE4CJJJCEYS1kD1brEi1WDaBsKYTHhL5WSq2gphW/p2HmeZbQ%2BR91IDRYtMWERFlfV9FgATREIsvwAaREOgAEkAC1LjO%2BZ7xqeREC1F03U67RNERAAPPrGsSEQFqWkQ8IAa1E/StIACXfRYAzDActMwAANEQ/rMP6tL%2BiNAcwV8BsWcGcEh9r0E6tberwaYBmALqerRwhFstMd/NZ9nIdbZGhd50XxZEVtRNBsibgGOhiKJE70ctCBdpiQ68HTDk2cSCGiNTeWRGoHRhdR43lDwKYx3VjtbnqFETud2g9smsWlsuw1el6HMBgAAUsflVP6RZUFGRhyvQU9ysq7mUeTv3%2BeXF0iWABlLBYQPxZOCOI/6SPzUW7R06q0tMFYE36847so5ryqs9LfTNsRSE87m/oc6zyvI9ZPA2jhXLNCJUEm/El2k3yjpJEkChNFwy13QwD0x4nqf%2BgoFhpFBO5GFS0Zlx34fm5d8v27eRZnmYSbnExFRHd6nOIBQagyn6MVUqAwuaJjqnBMqvMGJFnsi/HKTVNBu1WA2Y%2BzBpCs1BFzHmIt%2Ba9FMKeEQI9JZDkMKzeyvl%2BwkMCsg1uhZswFlUP0fy1CGxQMKkBNUtwizukDsASQYAwC2nLBFFcNBqowI4bue81IBiLFwoyJq80U5p3FPlXO%2BZnDw36DQbRbRS5LROEI64L5wyRmjBAfBhC760F/DrFaFiM7YNRqbTy0iRQDBGqI6gY1%2BbIFPNPIeoCkz5lYHgMaUwXLjGmkSfAhjzaghfNpXSBkLGpysYvWgJoLhBK5q45SMj%2BgjTGqyJ46ABT9GADgWMyhtH%2BO2heaUJ185Z1/PmU8o1xqvx4Ig/mjA144GAAEokrgEQ2DiU/TBaSc5ZMpFzUSvMyEcNuIYRkONmn62CWeIkTA4SSEYBQGQ29E66MhPg2%2BGS8DjPss6V06B3QiDhojTGqyIArAcIrfWzzEBYzycsJZ4cOFlUWO/AwJzu4j0HkSUFTSh76wfixK48TrgUOYbRGhCsRb62gfZApiwg6WjzFibxnR%2BboARjgMoiZQnhMmvI6J8JcFPxxcYkMpivzQC/krJaiZbr3Ues9N6H1vq/SZmVVs9isGcrwL0RovzhGcs%2BarMcvKHpPReu9T60Yfr/SBoshFJ4ZCnwoULe22hOU5yQaOdFCr8Uqx1tiwF7j474vhFMDeuFExTUUVCIgRIqrQkIJaJFpYXxEzMTGDFqN9aJg5R821Sq7AlhVfy9VQqtUiqBmba26AJWOKlTKuVZZI29UVTrHYiblV3VVQKjVwqdWYAwRWPibirjFq5WiSsRYTUO0VhaysDYxyWJzo21tdse0iwteWmBND2B6oKXQFeWJ%2BGCODfAyelrUR5XEfeTQ1BsYgHsrcAce7VmzXmi6yQ%2BYlxzQHrc90UwdmTUQKvHavivh%2Bt9VcjhNyYYPPhmSkQu7savMcHG5W2Nfm4g4ge/JyK%2ByoNPmihs4NEEOoRaCFFVCBGDqmdY027D0Nlkw8ObDNCh14ZmZgXJBGEosjZKkgh0zskZ0LQeZtjrSx0fQAx9JJtWOQXY/qwT9kD6EF2ZbRBZDhNLARI2EwcZ6poYw32GeipkFjkYCCz%2Bva8MgbsPjGQDIVh2BjeA%2BCd47B2CGkQZgqBjOypo2WbUCIKB6YTrvc5Ldr74s3NQezI61SibhB7JKgnQTOd1Hp8%2Bl9UT5hvhCuLPmsx%2Bcs/xoLp0pOjiWJ0ZozAQCdAzJ0YgehOjzCK8gfLjDL1tA6OW1Y7Ait4Hy2V2VxAsYcHOgIfLnAislbK8QCrnQiuSBAPMYgTXSs5eaNUwg7hFCcCAA%3D%3D%3D"></iframe>

To end this piece, I'll leave you with some supporting links for how to safely use goto for error handling, and why perhaps do/while(0) can cause problems (with loops):
- https://embeddedgurus.com/barr-code/2018/06/cs-goto-keyword-should-we-use-it-or-lose-it/
- https://ayende.com/blog/183521-C/error-handling-via-goto-in-c
- https://www.cprogramming.com/tutorial/goto.html
- https://embeddedgurus.com/stack-overflow/2010/02/goto-heresy/


Until next time!

-Micah
