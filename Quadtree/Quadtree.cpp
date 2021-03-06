#include "Quadtree.h"

const int Quadtree::MAX_LEVELS = 5;
const int Quadtree::DIVIDE_THRESHOLD = 10;

#define IDX_0 0x00000001
#define IDX_1 0x00000100
#define IDX_2 0x00010000
#define IDX_3 0x01000000

Quadtree::Quadtree(int level, SDL2pp::Rect bounds, Quadtree* parent) :
    level_(level), bounds_(bounds), children_(), actors_(), parent_(parent)
{
    children_.reserve(4);
    for (int i = 0; i < 4; i++) children_.push_back(nullptr);
    removed_.reserve(DIVIDE_THRESHOLD);
    reinserted_.reserve(DIVIDE_THRESHOLD);
}

void Quadtree::insert(std::shared_ptr<Actor>& actor)
{
    removed_.clear();
    if (children_[0] != nullptr)
    {
        std::uint32_t indices = getChildIndices(actor);
        int idx = getChildIndex(indices);
        if (idx != -1)
        {
            children_[idx]->insert(actor);
            return;
        }
    }

    actors_.push_back(actor);
    if (actors_.size() > DIVIDE_THRESHOLD && level_ < MAX_LEVELS)
    {
        if(children_[0] == nullptr)
            split();

        for(auto & a : actors_)
        {
            std::uint32_t indices = getChildIndices(a);
            int idx = getChildIndex(indices);
            if (idx != -1)
            {
                children_[idx]->insert(a);
            }
        }

        // Get rid of any moved items
        auto removedFinder = [&](auto& a) {return std::find(removed_.begin(), removed_.end(), &a) != removed_.end(); };
        actors_.erase(std::remove_if(actors_.begin(), actors_.end(), removedFinder), actors_.end());
    }
}

void Quadtree::update()
{
    reinserted_.clear();
    removed_.clear();
    if (children_[0] != nullptr)
    {
        children_[0]->update();
        children_[1]->update();
        children_[2]->update();
        children_[3]->update();
    }
    if (parent_ == nullptr) return;

    for (auto & actor : reinserted_)
    {
        std::uint32_t indices = getChildIndices(actor);
        int idx = getChildIndex(indices);
        if (idx != -1)
        {
            children_[idx]->insert(actor);
            return;
        }
        else
        {
            actors_.push_back(actor);
        }
    }

    for (auto & actor : actors_)
    {
        AABB &aabb = actor->_aabb;
        SDL2pp::Rect rect(aabb.GetX(), aabb.GetY(), aabb.GetWidth(), aabb.GetHeight());
        if (!bounds_.Contains(rect))
        {
            removed_.push_back(&actor);
        }
    }

    for (auto pa : removed_)
    {
        parent_->reinsert(*pa);
    }

    auto removedFinder = [&](auto& a) {return std::find(removed_.begin(), removed_.end(), &a) != removed_.end(); };
    actors_.erase(std::remove_if(actors_.begin(), actors_.end(), removedFinder), actors_.end());
}

void Quadtree::clear()
{
    actors_.clear();
    for (int i = 0; i < 4; ++i)
    {
        if (children_[i] != nullptr)
        {
            children_[i]->clear();
            children_[i] = nullptr;
        }
    }
}

void Quadtree::split()
{
    // This could theoretically cause some weird bugs because of integer division
    int childW = bounds_.w / 2;
    int childH = bounds_.h / 2;

    children_[0] = std::shared_ptr<Quadtree>(new Quadtree(level_ + 1, SDL2pp::Rect(bounds_.x, bounds_.y, childW, childH), this));
    children_[1] = std::shared_ptr<Quadtree>(new Quadtree(level_ + 1, SDL2pp::Rect(bounds_.x, bounds_.y + childH, childW, childH), this));
    children_[2] = std::shared_ptr<Quadtree>(new Quadtree(level_ + 1, SDL2pp::Rect(bounds_.x + childW, bounds_.y + childH, childW, childH), this));
    children_[3] = std::shared_ptr<Quadtree>(new Quadtree(level_ + 1, SDL2pp::Rect(bounds_.x + childW, bounds_.y, childW, childH), this));
}

void Quadtree::retrieve(std::vector<std::shared_ptr<Actor>*>& list, std::shared_ptr<Actor>& actor)
{
    std::uint32_t indices = getChildIndices(actor);

    if (children_[0] != nullptr)
    {
        if((indices & IDX_0) == IDX_0) children_[0]->retrieve(list, actor);
        if((indices & IDX_1) == IDX_1) children_[1]->retrieve(list, actor);
        if((indices & IDX_2) == IDX_2) children_[2]->retrieve(list, actor);
        if((indices & IDX_3) == IDX_3) children_[3]->retrieve(list, actor);
    }
    
    for (auto & a : actors_)
    {
        if (a != actor) list.push_back(&a);
    }
}

int Quadtree::getChildIndex(std::uint32_t indices) const
{
    {
        if ((indices & 0xffffffff) == IDX_0)      return 0;
        else if ((indices & 0xffffffff) == IDX_1) return 1;
        else if ((indices & 0xffffffff) == IDX_2) return 2;
        else if ((indices & 0xffffffff) == IDX_3) return 3;
        else                                      return -1;
    }
}

void Quadtree::draw(GameManager * mgr)
{
    if (!actors_.empty())
    {
        SDL_Point points[8];
        SDL2pp::Renderer &rend = mgr->GetRenderer();

        rend.SetScale(2, 2);
        rend.SetDrawColor(10 * level_, 10 * level_, 10 * level_, 255);
        rend.DrawRect(bounds_);
        rend.SetScale(1, 1);
    }
    if (children_[0] != nullptr)
    {
        children_[0]->draw(mgr);
        children_[1]->draw(mgr);
        children_[2]->draw(mgr);
        children_[3]->draw(mgr);
    }
}

void Quadtree::reinsert(std::shared_ptr<Actor>& actor)
{
    reinserted_.push_back(actor);
}

std::uint32_t Quadtree::getChildIndices(const SDL2pp::Rect& rect) const
{
    int midpointX = bounds_.x + (bounds_.w >> 1);
    int midpointY = bounds_.y + (bounds_.h >> 1);

    std::uint32_t indices = 0x01010101;

    static const unsigned int cases[] =
    {
        ~(IDX_1 | IDX_2),
        ~(IDX_0 | IDX_3),
        ~(IDX_3 | IDX_2),
        ~(IDX_1 | IDX_0)
    };
    int top = rect.y;
    int bottom = rect.y + rect.h;
    int left = rect.x;
    int right = rect.x + rect.w;

    if (top >= bounds_.y && bottom <= midpointY)
    {
        indices &= cases[0];
    }
    else if (top >= midpointY && bottom <= (bounds_.y + bounds_.h))
    {
        indices &= cases[1];
    }

    if (left >= bounds_.x && right <= midpointX)
    {
        indices &= cases[2];
    }
    else if (left >= midpointX && right <= (bounds_.x + bounds_.w))
    {
        indices &= cases[3];
    }
    return indices;
}

std::uint32_t Quadtree::getChildIndices(const std::shared_ptr<Actor>& a) const
{
    AABB &aabb = a->_aabb;
    SDL2pp::Rect rect(aabb.GetX(), aabb.GetY(), aabb.GetWidth(), aabb.GetHeight());
    return getChildIndices(rect);
}
