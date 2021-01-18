
Vue.component('system-component', {
  props: ['component'],
  methods: {
    css() {
      if (this.component.exclude) {
        return "system-component-not";
      }
      return "";
    }
  },
  template: `
      <span :class="'system-component ' + css()">
        <span class="system-component-annotation" v-if="component.const">
          in
        </span> 
        <span class="system-component-annotation" v-if="component.singleton">
          $
        </span> 
        <span class="system-component-annotation" v-else-if="component.parent">
          parent
        </span>                    
        <span class="system-component-annotation" v-else-if="component.ref">
          ref
        </span>    
        <span class="system-component-annotation" v-else-if="component.empty">
          0
        </span>                 
        {{component.name}}
      </span>`
});
