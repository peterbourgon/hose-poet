package main

import (
	"testing"
)

func TestSyllables(t *testing.T) {
	rh := NewRhyme(*rhymeBin)
	for word, expected := range map[string]int{
		"foo":           1,
		"define":        2,
		"observe":       2,
		"bifurcate":     3,
		"deliberate":    4,
		"automatically": 5,
	} {
		got, err := rh.Syllables(word)
		if err != nil {
			t.Errorf("%15s: %s", word, err)
			continue
		}
		if expected != got {
			t.Errorf("%15s: expected %d, got %d", word, expected, got)
			continue
		}
		t.Logf("%15s: %d OK", word, got)
	}
}
