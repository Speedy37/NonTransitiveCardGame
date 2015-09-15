#include <iostream>
#include <random>
#include <algorithm>
#include <vector>
#include <iostream>

typedef bool color;
const int nb_card = 52;
const int limit = nb_card / 2; // 26
const int tries = 1 << 20; // 2^14 << be gentle with online compiler (work fine at home with 2^22)
const bool count_draws = false;
struct ColorSet
{
	color a, b, c;

	static const std::vector<ColorSet> possibilities()
	{
		std::vector<ColorSet> p;
		p.push_back(ColorSet(0, 0, 0));
		p.push_back(ColorSet(0, 0, 1));
		p.push_back(ColorSet(0, 1, 0));
		p.push_back(ColorSet(0, 1, 1));
		p.push_back(ColorSet(1, 0, 0));
		p.push_back(ColorSet(1, 0, 1));
		p.push_back(ColorSet(1, 1, 0));
		p.push_back(ColorSet(1, 1, 1));
		return p;
	}

	ColorSet(color a, color b, color c)
	{
		this->a = a;
		this->b = b;
		this->c = c;
	}

	ColorSet best_counter()
	{
		return ColorSet(!b, a, b);
	}
};
std::ostream& operator<<(std::ostream& out, const ColorSet& s){
	return out << "(" << s.a << "," << s.b << "," << s.c << ")";
}


inline bool operator == (color *a, const ColorSet& b)
{
	return a[0] == b.a && a[1] == b.b && a[2] == b.c;
}
inline bool operator == (const ColorSet& b, color *a)
{
	return a == b;
}
inline bool operator == (const ColorSet& a, const ColorSet& b)
{
	return a.a == b.a && a.b == b.b && a.c == b.c;
}
inline bool operator != (const ColorSet& a, const ColorSet& b)
{
	return !(a == b);
}


class GameTester
{
private:
	std::vector<int> cards;
	std::mt19937 random_gen;

public:
	GameTester() {
		for (int i = 0; i < nb_card; ++i)
			cards.push_back(i);

		std::random_device rd;
		random_gen = std::mt19937(rd());
	}


	double compare(const ColorSet &a, const ColorSet &b, int nb_iteration)
	{
		int a_win = 0, b_win = 0;

		for (int i = 0; i < nb_iteration; ++i) {
			int stack_pos = 0;
			color stack[nb_card];
			int local_a_set = 0, local_b_set = 0;
			std::shuffle(cards.begin(), cards.end(), random_gen);
			for (int card_idx = 0; card_idx < nb_card; ++card_idx) {
				stack[stack_pos++] = cards[card_idx] < limit;
				if (stack_pos > 3) {
					color *stack_colors = stack + stack_pos - 3;
					if (stack_colors == a) {
						++local_a_set;
						stack_pos -= 3;
					}
					else if (stack_colors == b) {
						++local_b_set;
						stack_pos -= 3;
					}
				}
			}
			a_win += local_a_set > local_b_set ? 1 : 0;
			b_win += local_b_set > local_a_set ? 1 : 0;
		}

		// draws are not counted in
		return (double(a_win) / double(count_draws ? nb_iteration : a_win + b_win));
	}
};

int main() {
	GameTester gt;
	std::vector<ColorSet> possibilities = ColorSet::possibilities();
	double mean = 0.0;
	std::cout << "Testing " << possibilities.size() << " possibilities with " << tries << " round" << std::endl;
	std::for_each(possibilities.begin(), possibilities.end(), [&gt, &mean](ColorSet &b){
		ColorSet a = b.best_counter();
		double ret = gt.compare(a, b, tries);
		std::cout << a << " vs " << b << " -> " << ret * 100 << "%" << std::endl;
		mean += ret;
	});

	std::cout << "Counters win rate " << (mean / (double)possibilities.size()) * 100 << "%" << std::endl;

	std::cout << "Testing all possibilities" << std::endl;
	std::for_each(possibilities.begin(), possibilities.end(), [&gt, &possibilities](ColorSet &a){
		std::for_each(possibilities.begin(), possibilities.end(), [&a, &gt](ColorSet &b){
			if (a != b) {
				double ret = gt.compare(a, b, tries);
				std::cout << a << " vs " << b << " -> " << ret * 100 << "%" << std::endl;
			}
		});
	});

	return 0;
}

