
Vue.component('app-browser', {
  data: function() {
    return {
      scope: "",
      entity: "",
      entity_validated: "",
      entity_components: {},
      entity_request: undefined,
      error: false
    }
  },
  methods: {
    e_select_entity(event) {
      this.entity = event.entity;
      this.$refs.editor.select_entity(event.entity);
    },
    e_select_scope(event) {
      this.scope = event.scope;
      this.$refs.data.select_scope(event.scope);
    },
    e_select_component(event) {
      this.$refs.component_search.activate(event);
    }
  },
  template: `
    <div class="browser">
      <link rel="stylesheet" href="apps/browser/style.css">

      <div clas="entity-tree-container">
        <entity-tree 
          v-on:select-entity="e_select_entity" 
          v-on:select-scope="e_select_scope"
          :show_nav="true" 
          :auto_update="true">
        </entity-tree>   
      </div>

      <div class="entity-data-container">
        <entity-data 
          :scope="scope"
          ref="data">
        </entity-data>
      </div>
      
      <entity-editor
        :entity="entity" 
        :components="entity_components" 
        v-on:select-component="e_select_component"
        ref="editor">
      </entity-editor>

      <component-search ref="component_search">
      </component-search>        
    </div>
    `
});

// Signal app has loaded & pass on dependencies
app.app_loaded("browser", [{
    name: "entity-tree",
    url: "apps/browser/entity_tree.js"
  }, {
    name: "entity-tree-item",
    url: "apps/browser/entity_tree_item.js"
  }, {
    name: "entity-search",
    url: "apps/browser/entity_search.js"
  }, {
    name: "entity-kind",
    url: "apps/browser/entity_kind.js"
  }, {
    name: "entity-editor",
    url: "apps/browser/entity_editor.js"
  }, {
    name: "entity-component",
    url: "apps/browser/entity_component.js"
  }, {
    name: "component-properties",
    url: "apps/browser/component_properties.js"
  }, {
    name: "property-value",
    url: "apps/browser/property_value.js"
  }, {
    name: "entity-tag",
    url: "apps/browser/entity_tag.js"
  }, {
    name: "component-search",
    url: "apps/browser/component_search.js"
  }, {
    name: "entity-data",
    url: "apps/browser/entity_data.js"
  }, {
    name: "entity-table",
    url: "apps/browser/entity_table.js"
  }]
);
