- [x] add proper error handling to some rest api functions that dont already have it to prevent undefined behaviour
- [x] implement cv2 with comptime component layout checking via templates
- [x] automod api functions
- [x] user api functions
- [x] moderation api functions
- [x] message gateway events
- [ ] polls!
- [ ] audit log
- [x] forwarding
- [x] replying
- [ ] sharding
- [ ] guild templates!
- [ ] caching
- [x] basic guild gateway events
- [x] proper interaction callbacks using interactionCallbackTables for different types of interactions (button string select etc)<br>
this will be done by having a function like addInteractionCallback(const std::string_view name, std::function<void(T)> callback)<br>
T can be checked against different interaction data types to add them to the proper table without the user having to explicitly specify the interaction type
