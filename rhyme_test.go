package main

import (
	"testing"
)

func TestRhymes(t *testing.T) {
	type wordPair struct{ a, b string }
	for wordPair, expected := range map[wordPair]bool{
		wordPair{"cat", "bat"}:           true,
		wordPair{"cat", "bar"}:           false,
		wordPair{"incredible", "edible"}: true,
		wordPair{"slice", "mice"}:        true,
		wordPair{"grumpy", "stump"}:      false,
	} {
		if got := rhymes(wordPair.a, wordPair.b); got != expected {
			t.Errorf("%v: %v, expected %v", wordPair, got, expected)
		}
	}
}
