#Aho Corasick String Matching

Library implementsion

#Example Use

    char **keywords = (char **)malloc(sizeof(char *) * (ac - 1));
    ... fill in keywords
    g = init_goto();
    construct_goto(keywords, keyword_count, g);
	construct_failure(g);
    construct_delta(g);

    ...
    if (perform_match(g, input_string)) {
         // Matched one of the keywords   
    } else {
         // Did not match one of the keywords   
    }
    
    ...
    destroy_goto(g);

#Implementation

I wrote this directly from:

    String Matching: An Aid to Bibliographic Search 
    Alfred V Aho and Magaret J Corasick

It only matches ASCII keywords. It can handle UTF-8 input as
the string-to-be-matched, but not as the pattern keywords.
