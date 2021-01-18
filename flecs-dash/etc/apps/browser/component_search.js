
Vue.component('component-search', {
  data: function() {
      return {
          entity: "",
          visible: false,
          callback: undefined
      }
  },
  methods: {
    activate(event) {
      this.entity = "";
      this.$refs.tree.reset();
      this.visible = true;
      this.callback = event.callback;
    },
    deactivate() {
      this.visible = false;
      this.callback = undefined;
    },
    e_set_entity(event) {
      this.callback(event.entity);
      this.deactivate();
    }
  },
  computed: {
    visibility: function() {
      if (this.visible) {
        return "visible";
      } else {
        return "hidden";
      }
    }
  },
  template: `
    <div class="component-search-darken" :style="'visibility: ' + visibility" v-on:click="deactivate">
      <div class="component-search" v-on:click.stop="">
        <entity-tree
          v-on:select-entity="e_set_entity" 
          :filter="['Component', 'Module']" 
          :auto_update="false"
          search_text="Find a component"
          ref="tree">
        </entity-tree>
      </div>
    </div>
    `
});
