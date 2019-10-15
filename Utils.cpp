#include "Utils.h"

namespace Utils
{
    void clearLayout(QLayout* layout)
    {
        QLayoutItem* child;
        while(layout->count() != 0)
        {
           child = layout->takeAt(0);
           if(child->layout() != nullptr)
           {
               clearLayout(child->layout());
           }
           else if(child->widget() != nullptr)
           {
               delete child->widget();
           }
           delete child;
        }
    }
}
