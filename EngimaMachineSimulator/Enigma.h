// Enigma.h - Header-only Enigma machine core for MFC sample app (C++14)
//
// This module implements a simple, didactic Enigma M3 simulation with:
// - Plugboard
// -3 Rotors (choose from I..V)
// - Reflector (B or C)
// - Stepping with historical "double-stepping" behavior
//
// Design notes:
// - Characters are0..25 for A..Z. Non-alphabet characters are passed through unchanged by helpers in the UI.
// - Ring settings and rotor positions are supported.
// - Turnover notch is affected by ring setting (approximation used in historical simulations).
// - This file is header-only to avoid touching the project file list. Include it where needed.
//
// References for wirings (public domain sources):
// Rotor I : EKMFLGDQVZNTOWYHXUSPAIBRCJ, notch Q
// Rotor II : AJDKSIRUXBLHWTMCQGZNPYFVOE, notch E
// Rotor III: BDFHJLCPRTXVZNYEIWGAKMUSQO, notch V
// Rotor IV : ESOVPZJAYQUIRHXLNFTGKDCMWB, notch J
// Rotor V : VZBRGITYUPSDNHLXAWMJQOFECK, notch Z
// Reflector B: YRUHQSLDPXNGOKMIEBFZCWVJAT
// Reflector C: FVPJIAOYEDRZXWGCTKUQSBNMHL
//
// Extension ideas (see bottom): saving presets, visualization, keyboard lampboard, etc.

#pragma once

#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <random>

namespace EnigmaCore
{
 inline int mod26(int v) { v %=26; return v <0 ? v +26 : v; }
 inline int ch2i(char c) { return (c >= 'A' && c <= 'Z') ? (c - 'A') : (c >= 'a' && c <= 'z') ? (c - 'a') : -1; }
 inline char i2ch(int i) { return static_cast<char>('A' + mod26(i)); }

 struct Wiring
 {
 std::array<int,26> fwd{}; // forward mapping: input index -> output index
 std::array<int,26> rev{}; // reverse mapping: inverse of fwd

 static Wiring fromString(const std::string& s)
 {
 Wiring w{};
 for (int i =0; i <26; ++i)
 {
 int o = ch2i(s[(size_t)i]);
 w.fwd[(size_t)i] = o;
 }
 for (int i =0; i <26; ++i)
 {
 w.rev[(size_t)w.fwd[(size_t)i]] = i;
 }
 return w;
 }
 };

 // Plugboard: simple pair-swaps
 class Plugboard
 {
 public:
 Plugboard()
 {
 reset();
 }

 void reset()
 {
 for (int i =0; i <26; ++i) m_map[(size_t)i] = i;
 }

 // Configure from pairs like "AB CD EF" (whitespace ignored). Invalid pairs are ignored.
 void configureFromPairs(const std::string& pairs)
 {
 reset();
 char a =0, b =0;
 for (char c : pairs)
 {
 if (c == ' ' || c == '\t' || c == '\n' || c == '\r') continue;
 if (std::isalpha(static_cast<unsigned char>(c)))
 {
 if (!a) a = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
 else if (!b) b = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
 if (a && b)
 {
 int ia = ch2i(a), ib = ch2i(b);
 if (ia != -1 && ib != -1 && ia != ib)
 {
 // swap, but ensure neither already swapped
 if (m_map[(size_t)ia] == ia && m_map[(size_t)ib] == ib)
 {
 std::swap(m_map[(size_t)ia], m_map[(size_t)ib]);
 }
 }
 a = b =0;
 }
 }
 }
 }

 int map(int i) const { return m_map[(size_t)mod26(i)]; }

 private:
 std::array<int,26> m_map{};
 };

 class Rotor
 {
 public:
 Rotor() = default;
 Rotor(const Wiring& w, int notchIndex)
 : m_wiring(w), m_notch(notchIndex)
 {
 }

 void setPosition(int p) { m_pos = mod26(p); }
 void setRing(int r) { m_ring = mod26(r); }

 int position() const { return m_pos; }
 int ring() const { return m_ring; }

 // Returns true if rotor was at notch (causing turnover) considering ring setting.
 bool atNotch() const
 {
 // Approximate: turnover when (pos - ring) == notch
 return mod26(m_pos - m_ring) == m_notch;
 }

 // Advance rotor by one step
 void step() { m_pos = mod26(m_pos +1); }

 // Forward pass (right -> left)
 int forward(int i) const
 {
 int shifted = mod26(i + m_pos - m_ring);
 int wired = m_wiring.fwd[(size_t)shifted];
 return mod26(wired - m_pos + m_ring);
 }

 // Backward pass (left -> right)
 int backward(int i) const
 {
 int shifted = mod26(i + m_pos - m_ring);
 int wired = m_wiring.rev[(size_t)shifted];
 return mod26(wired - m_pos + m_ring);
 }

 private:
 Wiring m_wiring{};
 int m_notch{0 }; //0..25
 int m_pos{0 }; //0..25 (window letter A=0)
 int m_ring{0 }; //0..25 (ring setting A=0 -> historic ring=1)
 };

 class Reflector
 {
 public:
 Reflector() = default;
 explicit Reflector(const Wiring& w) : m_wiring(w) {}
 int map(int i) const { return m_wiring.fwd[(size_t)mod26(i)]; }
 private:
 Wiring m_wiring{};
 };

 class EnigmaMachine
 {
 public:
 EnigmaMachine() = default;

 void setRotors(const Rotor& left, const Rotor& middle, const Rotor& right)
 {
 m_left = left; m_middle = middle; m_right = right;
 }
 void setReflector(const Reflector& r) { m_reflector = r; }
 void setPlugboard(const Plugboard& p) { m_plug = p; }

 // Encrypt a single uppercase letter (A..Z). Other characters should be filtered by caller.
 char encryptChar(char c)
 {
 stepRotors();
 int x = ch2i(c);
 x = m_plug.map(x);
 x = m_right.forward(x);
 x = m_middle.forward(x);
 x = m_left.forward(x);
 x = m_reflector.map(x);
 x = m_left.backward(x);
 x = m_middle.backward(x);
 x = m_right.backward(x);
 x = m_plug.map(x);
 return i2ch(x);
 }

 // Utility to encrypt a whole string (letters only are transformed)
 std::string encrypt(const std::string& s)
 {
 std::string out; out.reserve(s.size());
 for (char c : s)
 {
 if (std::isalpha(static_cast<unsigned char>(c)))
 {
 out.push_back(encryptChar(static_cast<char>(std::toupper(static_cast<unsigned char>(c)))));
 }
 else
 {
 out.push_back(c);
 }
 }
 return out;
 }

 // Accessor to positions for UI
 int leftPos() const { return m_left.position(); }
 int midPos() const { return m_middle.position(); }
 int rightPos() const { return m_right.position(); }

 void setPositions(int left, int mid, int right)
 {
 m_left.setPosition(left);
 m_middle.setPosition(mid);
 m_right.setPosition(right);
 }

 private:
 // Implements the historical double-stepping for3-rotor machine
 void stepRotors()
 {
 bool rightAtNotch = m_right.atNotch();
 bool middleAtNotch = m_middle.atNotch();

 // Middle steps if it or right is at notch
 if (middleAtNotch || rightAtNotch)
 m_middle.step();
 // Left steps if middle was at notch
 if (middleAtNotch)
 m_left.step();
 // Right always steps
 m_right.step();
 }

 Rotor m_left, m_middle, m_right;
 Reflector m_reflector;
 Plugboard m_plug;
 };

 // Factory helpers for standard components
 inline Rotor RotorI() { return Rotor(Wiring::fromString("EKMFLGDQVZNTOWYHXUSPAIBRCJ"), ch2i('Q')); }
 inline Rotor RotorII() { return Rotor(Wiring::fromString("AJDKSIRUXBLHWTMCQGZNPYFVOE"), ch2i('E')); }
 inline Rotor RotorIII() { return Rotor(Wiring::fromString("BDFHJLCPRTXVZNYEIWGAKMUSQO"), ch2i('V')); }
 inline Rotor RotorIV() { return Rotor(Wiring::fromString("ESOVPZJAYQUIRHXLNFTGKDCMWB"), ch2i('J')); }
 inline Rotor RotorV() { return Rotor(Wiring::fromString("VZBRGITYUPSDNHLXAWMJQOFECK"), ch2i('Z')); }

 inline Reflector ReflectorB() { return Reflector(Wiring::fromString("YRUHQSLDPXNGOKMIEBFZCWVJAT")); }
 inline Reflector ReflectorC() { return Reflector(Wiring::fromString("FVPJIAOYEDRZXWGCTKUQSBNMHL")); }
}

// Extension ideas:
// - Persist and load configurations (rotor order, ring, positions, plugboard) using a simple file or registry.
// - Add a lampboard/keyboard visualisation: press keys to light cipher output letters.
// - Visualize stepping by drawing current rotor window letters and highlight when a notch triggers.
// - Support additional rotors (VI, VII, VIII) with two notches, and thin reflectors for M4.
// - Add a configuration dialog with validation and presets for historical keysheets.
