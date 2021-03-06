/*
 *  Copyright (C) 2014 Philippe Groarke.
 *  Author: Philippe Groarke <philippe.groarke@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.
 */

#ifndef WORD_H
#define WORD_H

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <list>
#include <map>
#include <math.h>
#include <random>
#include <unordered_set>
#include <vector>

using namespace std;

const string CHARACTER_ENDL = "END";
const string CHARACTER_BEGIN = "START";
const string CHARACTER_NAME = "NAME";

struct Word {
        Word() : word_(""), weight_(1) {}
        Word(const Word& obj) : word_(obj.word_), weight_(obj.weight_) {
                for (auto& x : obj.chain_) {
                        unique_ptr<Word> temp(new Word(*x.second));
                        chain_.insert(pair<string, unique_ptr<Word> >(
                                x.first, move(temp)));
                }
                for (auto x : obj.characteristics_) {
                        characteristics_.insert(x);
                }
        }
        Word(const string& txt) :
                word_(txt), weight_(1) {}

        // Will naturally stop at end of chain.
        void outputTopSentence(vector<unique_ptr<Word> >& vec, float randomPercent = 0.0f) {
                vec.push_back(unique_ptr<Word>(new Word(*this)));

                unique_ptr<Word> topWord(new Word());
                topWord->weight_ = 0;

                if (chain_.size() <= 0)
                        return;

                float percentageToContinue = 0.0;

                vector<unique_ptr<Word> > sortedVector;
                for (auto& x : chain_) {
                        percentageToContinue += x.second->weight_;
                        unique_ptr<Word> temp(new Word(*x.second));
                        sortedVector.push_back(move(temp));
                }
                sort(sortedVector.begin(), sortedVector.end(), [](unique_ptr<Word>& w1,
                        unique_ptr<Word>& w2) -> bool {
                        return w1->weight_ > w2->weight_;
                });
                percentageToContinue /= weight_;

                // Pick a random word in the top x percent.
                unsigned seed = chrono::system_clock::now().time_since_epoch().count();
                //mersenne_gen = mt19937(seed);

                int pickRange = randomPercent * chain_.size();
                default_random_engine generator(seed);
                uniform_int_distribution<int> distribution(0, pickRange);
                int pos = distribution(generator);
                topWord = move(sortedVector[pos]);

                // If the current word starts and ends a sentence, moving to another word
                // has to be at least used 25% of times.
                // if (characteristics_.find(CHARACTER_ENDL) != characteristics_.end()) {
                //         if (percentageToContinue > 0.25) {
                //                 distribution = uniform_int_distribution<int>(0, 100);
                //                 int luckyThrow = distribution(generator); // Maybe we get lucky?
                //                 if (luckyThrow > 100 * randomPercent) // Nope!
                //                         return; // We are a 1 word sentence, Kappa.
                //         }
                // }

                // Check if we have reached the end of a sentence.
                if (topWord->characteristics_.find(CHARACTER_ENDL)
                != topWord->characteristics_.end()) {
                        vec.push_back(move(topWord));
                        return;
                } else {
                        topWord->outputTopSentence(vec);
                }
        }

        void addWordInChain(list<unique_ptr<Word> >& wl) {
                if (wl.size() <= 0)
                    return;

                unordered_set<string> tempChar = wl.front()->characteristics_;

                auto ret = chain_.insert(
                        pair<string, unique_ptr<Word> >(
                                wl.front()->word_, move(wl.front())));

                if (ret.second == false) {
                        ret.first->second->weight_++;
                        // Add characteristics.
                        for (auto& x : tempChar)
                                ret.first->second->characteristics_.insert(x);
                }
                wl.pop_front();
                ret.first->second->addWordInChain(wl);
        }

        void printInfo(int indent = 0) {
                for (int i = 0; i < indent; ++i)
                        cout << "  ";

                string charact;
                for (const auto& x : characteristics_)
                        charact += " (" + x + ")";

                cout << "\"" << word_ << "\" " << weight_ << charact << endl;

                for (auto& x : chain_) {
                        x.second->printInfo(indent + 1);
                }
        }

        unique_ptr<Word>& getWord(const string& key) {
                return chain_.at(key);
        }

        unique_ptr<Word>& getWord(unique_ptr<Word>& key) {
                return chain_.at(key->word_);
        }

        unique_ptr<Word> topWord() {
                vector<unique_ptr<Word> > sortedByWeight;
                for (auto& x : chain_) {
                        unique_ptr<Word> temp(new Word(*x.second));
                        sortedByWeight.push_back(move(temp));
                }

                sort(sortedByWeight.begin(), sortedByWeight.end(), [](unique_ptr<Word>& w1,
                        unique_ptr<Word>& w2) -> bool {
                        return w1->weight_ > w2->weight_;
                });

                unique_ptr<Word> topWord(new Word());
                topWord->weight_ = 0;

                if (chain_.size() <= 0) {
                        return topWord;
                }

                srand (time(NULL));
                int top = rand() % 10;
                if (sortedByWeight.size()-1 < top) {
                        //cout << "Chain not long enough, size: " << chain_.size() << endl;
                        srand (time(NULL));
                        top = rand() % sortedByWeight.size();
                        //cout << "New random number: " << top << endl;
                }
                topWord.reset(new Word(*sortedByWeight[top]));

                return topWord;
        }

        friend ostream& operator<<(ostream& os, const Word& w) {
                os << w.word_ << endl << w.weight_ << endl;
                os << w.characteristics_.size() << endl;
                for (auto& x : w.characteristics_) {
                        os << x << endl;
                }

                os << w.chain_.size() << endl;
                for (auto& x : w.chain_) {
                        os << x.first << endl << *x.second;
                }
                return os;
        }

        friend istream& operator>>(istream& is, Word& w) {
                is >> w.word_ >> w.weight_;

                size_t size;
                is >> size;
                for (int i = 0; i < size; ++i) {
                        string c;
                        is >> c;
                        w.characteristics_.insert(c);
                }

                is >> size;
                for (int i = 0; i < size; ++i) {
                        string key;
                        unique_ptr<Word> temp(new Word());
                        is >> key >> *temp;
                        w.chain_.insert(
                                pair<string, unique_ptr<Word> >(key, move(temp)));
                }

                return is;
        }

        map<string, unique_ptr<Word> > chain_;
        unordered_set<string> characteristics_;

        string word_;
        int weight_ = 1;

};

#endif // WORD_H
