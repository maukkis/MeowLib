- [ ] implement cv2 with comptime component layout checking via templates
- [ ] automod api functions
- [ ] user api functions
- [ ] moderation api functions
- [ ] proper interaction callbacks using interactionCallbackTables for different types of interactions (button string select etc)<br>
this will be done by having a function like addInteractionCallback(const std::string_view name, std::function<void(T)> callback)<br>
T can be checked against different interaction data types to add them to the proper table without the user having to explicitly specify the interaction type
