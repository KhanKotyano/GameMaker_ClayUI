
void Clay__ConfigureOpenElementDynamic(Clay_ElementDeclaration declaration) {
    Clay_Context* context = Clay_GetCurrentContext();
    Clay_LayoutElement *openLayoutElement = Clay__GetOpenLayoutElement();
    openLayoutElement->layoutConfig = Clay__StoreLayoutConfig(declaration.layout);
    if (declaration.id.id != 0) {
        Clay__AttachId(declaration.id);
    } else if (openLayoutElement->id == 0) {
        Clay__GenerateIdForAnonymousElement(openLayoutElement);
    }

    openLayoutElement->elementConfigs.internalArray = &context->elementConfigs.internalArray[context->elementConfigs.length];
    Clay_SharedElementConfig *sharedConfig = NULL;
    if (declaration.backgroundColor.a > 0) {
        sharedConfig = Clay__StoreSharedElementConfig(CLAY__INIT(Clay_SharedElementConfig) { .backgroundColor = declaration.backgroundColor });
        Clay__AttachElementConfig(CLAY__INIT(Clay_ElementConfigUnion) { .sharedElementConfig = sharedConfig }, CLAY__ELEMENT_CONFIG_TYPE_SHARED);
    }
    if (!Clay__MemCmp((char *)(&declaration.cornerRadius), (char *)(&Clay__CornerRadius_DEFAULT), sizeof(Clay_CornerRadius))) {
        if (sharedConfig) {
            sharedConfig->cornerRadius = declaration.cornerRadius;
        } else {
            sharedConfig = Clay__StoreSharedElementConfig(CLAY__INIT(Clay_SharedElementConfig) { .cornerRadius = declaration.cornerRadius });
            Clay__AttachElementConfig(CLAY__INIT(Clay_ElementConfigUnion) { .sharedElementConfig = sharedConfig }, CLAY__ELEMENT_CONFIG_TYPE_SHARED);
        }
    }
    if (declaration.userData != 0) {
        if (sharedConfig) {
            sharedConfig->userData = declaration.userData;
        } else {
            sharedConfig = Clay__StoreSharedElementConfig(CLAY__INIT(Clay_SharedElementConfig) { .userData = declaration.userData });
            Clay__AttachElementConfig(CLAY__INIT(Clay_ElementConfigUnion) { .sharedElementConfig = sharedConfig }, CLAY__ELEMENT_CONFIG_TYPE_SHARED);
        }
    }
    if (declaration.image.imageData) {
        Clay__AttachElementConfig(CLAY__INIT(Clay_ElementConfigUnion) { .imageElementConfig = Clay__StoreImageElementConfig(declaration.image) }, CLAY__ELEMENT_CONFIG_TYPE_IMAGE);
        Clay__int32_tArray_Add(&context->imageElementPointers, context->layoutElements.length - 1);
    }
    if (declaration.floating.attachTo != CLAY_ATTACH_TO_NONE) {
        Clay_FloatingElementConfig floatingConfig = declaration.floating;
        // This looks dodgy but because of the auto generated root element the depth of the tree will always be at least 2 here
        Clay_LayoutElement *hierarchicalParent = Clay_LayoutElementArray_Get(&context->layoutElements, Clay__int32_tArray_GetValue(&context->openLayoutElementStack, context->openLayoutElementStack.length - 2));
        if (hierarchicalParent) {
            uint32_t clipElementId = 0;
            if (declaration.floating.attachTo == CLAY_ATTACH_TO_PARENT) {
                // Attach to the element's direct hierarchical parent
                floatingConfig.parentId = hierarchicalParent->id;
                if (context->openClipElementStack.length > 0) {
                    clipElementId = Clay__int32_tArray_GetValue(&context->openClipElementStack, (int)context->openClipElementStack.length - 1);
                }
            } else if (declaration.floating.attachTo == CLAY_ATTACH_TO_ELEMENT_WITH_ID) {
                Clay_LayoutElementHashMapItem *parentItem = Clay__GetHashMapItem(floatingConfig.parentId);
                if (!parentItem) {
                    context->errorHandler.errorHandlerFunction(CLAY__INIT(Clay_ErrorData) {
                            .errorType = CLAY_ERROR_TYPE_FLOATING_CONTAINER_PARENT_NOT_FOUND,
                            .errorText = CLAY_STRING("A floating element was declared with a parentId, but no element with that ID was found."),
                            .userData = context->errorHandler.userData });
                } else {
                    clipElementId = Clay__int32_tArray_GetValue(&context->layoutElementClipElementIds, parentItem->layoutElement - context->layoutElements.internalArray);
                }
            } else if (declaration.floating.attachTo == CLAY_ATTACH_TO_ROOT) {
                floatingConfig.parentId = Clay__HashString(CLAY_STRING("Clay__RootContainer"), 0, 0).id;
            }
            Clay__LayoutElementTreeRootArray_Add(&context->layoutElementTreeRoots, CLAY__INIT(Clay__LayoutElementTreeRoot) {
                    .layoutElementIndex = Clay__int32_tArray_GetValue(&context->openLayoutElementStack, context->openLayoutElementStack.length - 1),
                    .parentId = floatingConfig.parentId,
                    .clipElementId = clipElementId,
                    .zIndex = floatingConfig.zIndex,
            });
            Clay__AttachElementConfig(CLAY__INIT(Clay_ElementConfigUnion) { .floatingElementConfig = Clay__StoreFloatingElementConfig(declaration.floating) }, CLAY__ELEMENT_CONFIG_TYPE_FLOATING);
        }
    }
    if (declaration.custom.customData) {
        Clay__AttachElementConfig(CLAY__INIT(Clay_ElementConfigUnion) { .customElementConfig = Clay__StoreCustomElementConfig(declaration.custom) }, CLAY__ELEMENT_CONFIG_TYPE_CUSTOM);
    }
    if (declaration.scroll.horizontal | declaration.scroll.vertical) {
        Clay__AttachElementConfig(CLAY__INIT(Clay_ElementConfigUnion) { .scrollElementConfig = Clay__StoreScrollElementConfig(declaration.scroll) }, CLAY__ELEMENT_CONFIG_TYPE_SCROLL);
        Clay__int32_tArray_Add(&context->openClipElementStack, (int)openLayoutElement->id);
        // Retrieve or create cached data to track scroll position across frames
        Clay__ScrollContainerDataInternal *scrollOffset = CLAY__NULL;
        for (int32_t i = 0; i < context->scrollContainerDatas.length; i++) {
            Clay__ScrollContainerDataInternal *mapping = Clay__ScrollContainerDataInternalArray_Get(&context->scrollContainerDatas, i);
            if (openLayoutElement->id == mapping->elementId) {
                scrollOffset = mapping;
                scrollOffset->layoutElement = openLayoutElement;
                scrollOffset->openThisFrame = true;
            }
        }
        if (!scrollOffset) {
            scrollOffset = Clay__ScrollContainerDataInternalArray_Add(&context->scrollContainerDatas, CLAY__INIT(Clay__ScrollContainerDataInternal){.layoutElement = openLayoutElement, .scrollOrigin = {-1,-1}, .elementId = openLayoutElement->id, .openThisFrame = true});
        }
        if (context->externalScrollHandlingEnabled) {
            scrollOffset->scrollPosition = Clay__QueryScrollOffset(scrollOffset->elementId, context->queryScrollOffsetUserData);
        }
    }
    if (!Clay__MemCmp((char *)(&declaration.border.width), (char *)(&Clay__BorderWidth_DEFAULT), sizeof(Clay_BorderWidth))) {
        Clay__AttachElementConfig(CLAY__INIT(Clay_ElementConfigUnion) { .borderElementConfig = Clay__StoreBorderElementConfig(declaration.border) }, CLAY__ELEMENT_CONFIG_TYPE_BORDER);
    }
}



static Clay_ElementDeclaration current_elemet= {0};
#define dCLAY(var) \
	for (\
		CLAY__ELEMENT_DEFINITION_LATCH = (Clay__OpenElement(), Clay__ConfigureOpenElementDynamic(var), 0); \
		CLAY__ELEMENT_DEFINITION_LATCH < 1; \
		++CLAY__ELEMENT_DEFINITION_LATCH, Clay__CloseElement() \
	)
DLLEXPORT double gml_clay_element_config(){
    Clay__OpenElement();
    Clay__ConfigureOpenElementDynamic(current_elemet);
    return 1;
}

DLLEXPORT double gml_clay_ui_element_set_border( double border_w){
    current_elemet.border.width.bottom = border_w;
    current_elemet.border.width.right = border_w;
    current_elemet.border.width.left = border_w;
    current_elemet.border.width.top = border_w;
   
    return 1;
}
DLLEXPORT double gml_clay_ui_element_set_color( double color, double alpha){
    UniColor _col= {.uni = (u32)color};
    current_elemet.border.color= UNI_COLOR_TO_CLAY_COLOR(_col);;
    current_elemet.border.color.a= 255 * alpha;
   
    return 1;
}
//images
//s32 image_id_array[10000];
DLLEXPORT double gml_clay_ui_element_set_image_id( double image_id){
    current_elemet.image.imageData = (s32)image_id;
   
    return 1;
}
//floating elements
DLLEXPORT double gml_clay_ui_element_floating_attach_to_parent( double attach_point){
    current_elemet.floating.attachTo = CLAY_ATTACH_TO_PARENT;
    current_elemet.floating.attachPoints.parent =(Clay_FloatingAttachPointType)attach_point ;
   
    return 1;
}
DLLEXPORT double gml_clay_ui_element_attach_to_point( double point_x, double point_y){
    current_elemet.floating.attachTo = CLAY_ATTACH_TO_ROOT;
    current_elemet.floating.offset.x = point_x ;
    current_elemet.floating.offset.y = point_y ;
   
    return 1;
}
//TODO write Clay_FloatingAttachPointType enums in gml

//
//text
static Clay_TextElementConfig current_text_config = {0};
DLLEXPORT double gml_clay_set_text_spacing(double f_space, double f_line_h,double f_wrapmode){
    current_text_config.letterSpacing = (u16)f_space;
    current_text_config.lineHeight = (u16)f_line_h;
    current_text_config.wrapMode = (Clay_TextElementConfigWrapMode)f_wrapmode;
    return 1;
}
DLLEXPORT double gml_clay_set_text_font(double f_id, double f_size){
    current_text_config.fontId = (u16)f_id;
    current_text_config.fontSize = (u16)f_size;
    return 1;
}
DLLEXPORT double gml_clay_set_text_color(double color, double alpha){
    UniColor _col= {.uni = (u32)color};
    current_text_config.textColor = UNI_COLOR_TO_CLAY_COLOR(_col);
    current_text_config.textColor.a = 255 * alpha;
    return 1;
}
DLLEXPORT double gml_clay_element_set_text(char *gml_string){
    s32 length = strlen(gml_string);
    Clay_String clay_str = {.chars = gml_string, .length = (s32)length};
    CLAY_TEXT(clay_str, Clay__StoreTextElementConfig(current_text_config));
    //ZeroMemory_Util(&current_text_config, sizeof(current_text_config));
    return 1;
}
//end of text function
void RenderHeaderButton(Clay_String text) {
    CLAY({
        .layout = { 
          // .sizing = {
          //   .height = CLAY_SIZING_FIXED(60),
          //   .width = CLAY_SIZING_GROW(60)
          // },
        .padding = { 16, 16, 8, 8 }
        },
        .backgroundColor = { 140, 140, 140, 255 },
        .cornerRadius = CLAY_CORNER_RADIUS(5)
    }) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = 0,
            .fontSize = 16,
            .textColor = { 255, 255, 255, 255 }
        }));
    }
  }
void RenderDropdownMenuItem(Clay_String text) {
    CLAY({.layout = { .padding = CLAY_PADDING_ALL(16)}}) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = 0,
            .fontSize = 16,
            .textColor = { 255, 255, 255, 255 }
        }));
    }
  }




static int open_elements = 0;

static Clay_RenderCommandArray render_command_array = {0};

static Clay_RenderCommandArray ClayGetRenderCommands(){
    return render_command_array;
}
DLLEXPORT double gml_clay_begin_layout(){
    open_elements = 0;
    m_requests.counter = 0;
    ZeroMemory_Util(&current_elemet, sizeof(Clay_ElementDeclaration));
    Clay_BeginLayout();
    return 1;
}
DLLEXPORT double gml_clay_end_layout(double update_commands){
    //m_requests.counter = 0;
    //if(update_commands) {render_command_array = Clay_EndLayout();} else {Clay_EndLayout();}
    render_command_array = Clay_EndLayout();
    return 1;
}
static u64 anon_el_counter = 0;
DLLEXPORT double gml_clay_open_ui_element(char *name){
    ZeroMemory_Util(&current_elemet, sizeof(Clay_ElementDeclaration));
    open_elements++;
    Clay_String _string = {0};
    if(name != 0){
        _string.length = strlen(name);
        _string.chars = name;
        current_elemet.id = Clay__HashString(_string,0,Clay__GetParentElementId());
    } 
    return current_elemet.id.id;
}
DLLEXPORT double gml_clay_set_ui_element(){
    if( open_elements)Clay__ConfigureOpenElementDynamic(current_elemet);
    if(open_elements) Clay__CloseElement();
    open_elements--;
    return 1;
}
DLLEXPORT double gml_clay_close_ui_element(){
    Clay__CloseElement();
    return 1;
}
DLLEXPORT double gml_clay_ui_element_background_color(double color, double alpha){
    UniColor _col= {.uni = (u32)color};
    current_elemet.backgroundColor = UNI_COLOR_TO_CLAY_COLOR(_col);
    current_elemet.backgroundColor.a = 255 * alpha;
    return 1;
}

typedef enum {
    SIZING_GROW = 0,
    SIZING_FIT,
    SIZING_PERCENT,
    SIZING_FIXED
}INTERAL_CLAY_LAYOUT_SIZING;
DLLEXPORT double gml_clay_ui_element_layout_sizing(double w_type, double h_type, double w_var, double h_var, double radius){
    switch((u32)w_type){
        case SIZING_GROW:{ current_elemet.layout.sizing.width = CLAY_SIZING_GROW(w_var);}break;
        case SIZING_FIT:{current_elemet.layout.sizing.width = CLAY_SIZING_FIT(w_var);}break;
        case SIZING_PERCENT:{current_elemet.layout.sizing.width = CLAY_SIZING_PERCENT(w_var);}break;
        case SIZING_FIXED:{current_elemet.layout.sizing.width = CLAY_SIZING_FIXED(w_var);}break;
    }
    switch((u32)h_type){
        case SIZING_GROW:{ current_elemet.layout.sizing.height = CLAY_SIZING_GROW(h_var);}break;
        case SIZING_FIT:{current_elemet.layout.sizing.height = CLAY_SIZING_FIT(h_var);}break;
        case SIZING_PERCENT:{current_elemet.layout.sizing.height = CLAY_SIZING_PERCENT(h_var);}break;
        case SIZING_FIXED:{current_elemet.layout.sizing.height = CLAY_SIZING_FIXED(h_var);}break;
    }
    current_elemet.cornerRadius = CLAY_CORNER_RADIUS(radius);
    return 1;
}
DLLEXPORT double gml_clay_ui_element_layout_padding(double bottom, double left, double right, double top){
    current_elemet.layout.padding.bottom = bottom;
    current_elemet.layout.padding.left = left;
    current_elemet.layout.padding.right = right;
    current_elemet.layout.padding.top = top;
    return 1;
}
DLLEXPORT double gml_clay_ui_element_layout_padding_all(double padding){
    current_elemet.layout.padding = CLAY_PADDING_ALL(padding);
    return 1;
}
//TODO write Clay_LayoutAlignmentX and Clay_LayoutAlignmentY enums in gml
DLLEXPORT double gml_clay_ui_element_layout_child(double child_gap,double allign_x, double allign_y){
    current_elemet.layout.childGap = child_gap;
    current_elemet.layout.childAlignment.x  = allign_x;
    current_elemet.layout.childAlignment.y  = allign_y;
    return 1;
}
//TODO make an enum for Clay_LayoutDirection in gml
DLLEXPORT double gml_clay_ui_element_layout_direction(double dir){
    current_elemet.layout.layoutDirection = (Clay_LayoutDirection)dir;
    return 1;
}
// static Clay_RenderCommandArray ClayGetRenderCommands_NativeC(){
//     Clay_BeginLayout();
//     Clay_Sizing layoutExpand = {
//         .width = CLAY_SIZING_GROW(0),
//         .height = CLAY_SIZING_GROW(0)
//       };


//     CLAY({ .id = CLAY_ID("OuterContainer"),
//       .backgroundColor = {43, 41, 51, 0 },
//       .layout = {
//           .layoutDirection = CLAY_TOP_TO_BOTTOM,
//           .sizing = layoutExpand,
//           .padding = CLAY_PADDING_ALL(16),
//           .childGap = 16
//       }}){
//         CLAY({.id = 0});
//       };
//     // }) {
//     //   // Child elements go inside braces
//     //   CLAY({ .id = CLAY_ID("HeaderBar"),
//     //       .layout = {
//     //           .sizing = {
//     //               .height = CLAY_SIZING_FIXED(60),
//     //               .width = CLAY_SIZING_GROW(0)
//     //           },
//     //           .padding = { 16, 16, 0, 0 },
//     //           .childGap = 16,
//     //           .childAlignment = {
//     //               .y = CLAY_ALIGN_Y_CENTER
//     //           }
//     //       },
//     //       .backgroundColor = GRAY_UNI(Clay_Color),
//     //       .cornerRadius = CLAY_CORNER_RADIUS(8)
//     //   }) {
//     //       // Header buttons go here
//     //       CLAY({ .id = CLAY_ID("FileButton"),
//     //           .layout = { .padding = { 16, 16, 8, 8 }},
//     //           .backgroundColor = {140, 140, 140, 255 },
//     //           .cornerRadius = CLAY_CORNER_RADIUS(5)
//     //       }) {
//     //           CLAY_TEXT(CLAY_STRING("File"), CLAY_TEXT_CONFIG({
//     //               .fontId = 0,
//     //               .fontSize = 16,
//     //               .textColor = { 255, 255, 255, 255 }
//     //           }));
  
//     //           bool fileMenuVisible =
//     //               Clay_PointerOver(Clay_GetElementId(CLAY_STRING("FileButton")))
//     //               ||
//     //               Clay_PointerOver(Clay_GetElementId(CLAY_STRING("FileMenu")));
  
//     //           if (fileMenuVisible) { // Below has been changed slightly to fix the small bug where the menu would dismiss when mousing over the top gap
//     //               CLAY({ .id = CLAY_ID("FileMenu"),
//     //                   .floating = {
//     //                       .attachTo = CLAY_ATTACH_TO_PARENT,
//     //                       .attachPoints = {
//     //                           .parent = CLAY_ATTACH_POINT_LEFT_BOTTOM
//     //                       },
//     //                   },
//     //                   .layout = {
//     //                       .padding = {0, 0, 8, 8 }
//     //                   }
//     //               }) {
//     //                   CLAY({
//     //                       .layout = {
//     //                           .layoutDirection = CLAY_TOP_TO_BOTTOM,
//     //                           .sizing = {
//     //                                   .width = CLAY_SIZING_FIXED(200)
//     //                           },
//     //                       },
//     //                       .backgroundColor = {40, 40, 40, 255 },
//     //                       .cornerRadius = CLAY_CORNER_RADIUS(8)
//     //                   }) {
//     //                       // Render dropdown items here
//     //                       RenderDropdownMenuItem(CLAY_STRING("New"));
//     //                       RenderDropdownMenuItem(CLAY_STRING("Open"));
//     //                       RenderDropdownMenuItem(CLAY_STRING("Close"));
//     //                   }
//     //               }
//     //           }
//     //       }
//     //       RenderHeaderButton(CLAY_STRING("Edit"));
//     //       CLAY({ .layout = { .sizing = { CLAY_SIZING_GROW(0) }}}) {}
//     //       RenderHeaderButton(CLAY_STRING("Upload"));
//     //       RenderHeaderButton(CLAY_STRING("Media"));
//     //       RenderHeaderButton(CLAY_STRING("Support"));
//     //   }
//     // }
  
//     Clay_RenderCommandArray renderCommands = Clay_EndLayout();
//     return renderCommands;
//   }  