Vue.component('sidebar-button', {
  props: {
    app: String,
    icon: String,
    active_app: String
  },
  data: function() {
    return {
      hover: false
    }
  },
  methods: {
    selector_css() {
      let result = "sidebar-selector";
      if (this.app == this.active_app) {
        result += " sidebar-selector-active";
      } else if (this.hover) {
        result += " sidebar-selector-hover";
      }
      return result;
    },    
    button_css() {
      return "sidebar-button";
    },
    select_app: function() {
      this.$emit('select-app', {name: this.app});
    },
    set_hover(hover) {
      this.hover = hover;
    }
  },
  template: `
    <div class="sidebar-button-container">
      <div :class="selector_css()">
      </div>

      <div :class="button_css()" 
        v-on:click="select_app"
        v-on:mouseover="set_hover(true)"
        v-on:mouseleave="set_hover(false)"> 

        <img :src="icon" class="sidebar-icon">
      </div>
    </div>
    `
});
