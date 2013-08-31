package main

import (
	"fmt"
	"github.com/peterbourgon/go-hyphenator"
	"strings"
)

var myHyphenator = hyphenator.NewHyphenator()

const hyphen = "-"

func meter(s string) (int, error) {
	meter := 0
	for _, word := range strings.Split(s, " ") {
		hyphenated, ok := myHyphenator.Hyphenate(word, hyphen)
		if !ok {
			return 0, fmt.Errorf("hyphenation failed on '%s'", word)
		}
		meter += 1 + strings.Count(hyphenated, hyphen)
	}
	return meter, nil
}
