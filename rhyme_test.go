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

func TestMatch(t *testing.T) {
	rh := NewRhyme(*rhymeBin)

	for _, tu := range []struct {
		C1       string
		C2       string
		Expected bool
	}{
		{"1-foo", "1-foo", true},
		{"1-foo", "1-blue", true},
		{"1-foo", "2-reclaim", false},
		{"19-disdain", "19-refrain", true},
		{"2-reclaim", "9-disdain", false},
		{"8-nice", "8-twice", true},
	} {
		if got := rh.Match(tu.C1, tu.C2); tu.Expected != got {
			t.Errorf("%15s %15s: expected %v, got %v", tu.C1, tu.C2, tu.Expected, got)
			continue
		}
		t.Logf("%15s %15s: %v OK", tu.C1, tu.C2, tu.Expected)
	}
}
